#include "FZstdShaderCompressionFormat.h"

#include "zstdmt_compress.h"
#include "zstd_compress_internal.h"
#include "zstd_decompress_internal.h"
#include "zstd_ldm.h"
#include "Misc/Paths.h"

FZstdShaderCompressionFormat::FZstdShaderCompressionFormat(const FString& InDictDir):DictDir(InDictDir)
{
	if(FPaths::FileExists(InDictDir))
	{
		TArray<uint8> DictData;
		bDictLoaded = FFileHelper::LoadFileToArray(DictData,*DictDir);
		if(bDictLoaded)
		{
			UE_LOG(LogTemp,Display,TEXT("zstd compressor dict loaded!!"));
			CDict = ZSTD_createCCtx();
			ZSTD_CCtx_loadDictionary(CDict,(void*)DictData.GetData(),DictData.Num());
			DDict = ZSTD_createDCtx();
			ZSTD_DCtx_loadDictionary(DDict,(void*)DictData.GetData(),DictData.Num());
		}
	}
}

FZstdShaderCompressionFormat::~FZstdShaderCompressionFormat()
{
	if(CDict)
	{
		ZSTD_freeCCtx(CDict);
	}
	if(DDict)
	{
		ZSTD_freeDCtx(DDict);
	}
}

FName FZstdShaderCompressionFormat::GetCompressionFormatName()
{
	return TEXT("zstdshader");
}

bool FZstdShaderCompressionFormat::Compress(void* CompressedBuffer, int32& CompressedSize,
	const void* UncompressedBuffer, int32 UncompressedSize, int32 CompressionData)
{
	SCOPED_NAMED_EVENT_TEXT("FZstdShaderCompressionFormat::Compress",FColor::Red);
	if(IsDictLoaded())
	{
		// small size (1kb
		int32 Result = ZSTD_compress_usingDict(CDict, CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize, CDict->localDict.dict, CDict->localDict.dictSize, 22);
		// large size (2.4kb
		// int32 Result = ZSTD_compressCCtx(CDict,CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize,22);
		if (!ZSTD_isError(Result))
		{
			if (Result > GetCompressedBufferSize(UncompressedSize, CompressionData))
			{
				FPlatformMisc::LowLevelOutputDebugStringf(TEXT("%d < %d"), Result, GetCompressedBufferSize(UncompressedSize, CompressionData));
				// we cannot safely go over the BufferSize needed!
				return false;
			}
			CompressedSize = Result;
			return true;
		}
	}else
	{
		return FCompression::CompressMemory(NAME_LZ4,CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize);
	}
	return false;	
}

bool FZstdShaderCompressionFormat::Uncompress(void* UncompressedBuffer, int32& UncompressedSize,
	const void* CompressedBuffer, int32 CompressedSize, int32 CompressionData)
{
	SCOPED_NAMED_EVENT_TEXT("FZstdShaderCompressionFormat::Uncompress",FColor::Red);
	if(IsDictLoaded())
	{
		auto Result = ZSTD_decompressDCtx(DDict, UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
		if (!ZSTD_isError(Result))
		{
			UncompressedSize = Result;
			return true;
		}
		return false;	
	}else{
		return FCompression::UncompressMemory(NAME_LZ4,UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
	}
}