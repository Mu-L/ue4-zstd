// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "libzstd.h"

#include "FZstdShaderCompressionFormat.h"


#define LOCTEXT_NAMESPACE "FlibzstdModule"
#define DEFAULT_COMPRESSION_LEVEL 10

int32 FZstdCompressionFormat::Level = DEFAULT_COMPRESSION_LEVEL;

#define ZSTD_LEVEL_OPTION_STRING TEXT("-ZstdLevel=")
void FlibzstdModule::StartupModule()
{
	FString CommandLine = FCommandLine::Get();
	if (CommandLine.Contains(ZSTD_LEVEL_OPTION_STRING, ESearchCase::IgnoreCase))
	{
		int32 level;
		FParse::Value(FCommandLine::Get(), *FString(ZSTD_LEVEL_OPTION_STRING).ToLower(), level);
		FZstdCompressionFormat::Level = FMath::Clamp(level, ZSTD_minCLevel(),ZSTD_maxCLevel());
	}

	UE_LOG(LogTemp, Log, TEXT("FZstdCompressionFormat::Compress level is %d"), FZstdCompressionFormat::Level);
	
	auto ZstdCompressionFormat = new FZstdCompressionFormat();
	ZstdCompressionFormats.Add(ZstdCompressionFormat->GetCompressionFormatName(),ZstdCompressionFormat);
	
	FString DictPath = FPaths::Combine(FPaths::ProjectContentDir(),TEXT("shader.dict"));
	auto ZstdShaderCompressionFormat = new FZstdShaderCompressionFormat(DictPath);
	ZstdCompressionFormats.Add(ZstdShaderCompressionFormat->GetCompressionFormatName(),ZstdShaderCompressionFormat);

	for(auto Format:ZstdCompressionFormats)
	{
		if(Format.Value)
		{
			IModularFeatures::Get().RegisterModularFeature(COMPRESSION_FORMAT_FEATURE_NAME, Format.Value);
		}
	}
}

void FlibzstdModule::ShutdownModule()
{
	for(auto Format:ZstdCompressionFormats)
	{
		if(Format.Value)
		{
			IModularFeatures::Get().UnregisterModularFeature(COMPRESSION_FORMAT_FEATURE_NAME, Format.Value);
			delete Format.Value;
		}
		
		Format.Value = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FlibzstdModule, libzstd)