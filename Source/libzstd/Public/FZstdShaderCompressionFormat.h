#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "FZstdCompressionFormat.h"

struct LIBZSTD_API FZstdShaderCompressionFormat : public FZstdCompressionFormat
{
	FZstdShaderCompressionFormat(const FString& InDictDir);
	virtual ~FZstdShaderCompressionFormat();
	virtual FName GetCompressionFormatName()override;
	virtual bool Compress(void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize, int32 CompressionData)override;
	virtual bool Uncompress(void* UncompressedBuffer, int32& UncompressedSize, const void* CompressedBuffer, int32 CompressedSize, int32 CompressionData)override;
	static int32 Level;
private:
	bool IsDictLoaded()const { return bDictLoaded; }
	FString DictDir;
	bool bDictLoaded = false;
	ZSTD_CCtx* CDict = nullptr;
	ZSTD_DCtx* DDict = nullptr;
};


