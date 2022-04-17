#include "FZstdShaderCompressionFormat.h"

#include "zstdmt_compress.h"
#include "zstd_compress_internal.h"
#include "zstd_decompress_internal.h"
#include "zstd_ldm.h"
#include "Misc/Paths.h"

static FName DefaultShaderCompressionName = NAME_LZ4;

DEFINE_LOG_CATEGORY_STATIC(LogZstdShader,All,All);

FZstdShaderCompressionFormat::FZstdShaderCompressionFormat(const FString& InDictDir):DictDir(InDictDir)
{
	if(FPaths::FileExists(InDictDir))
	{
		TArray<uint8> DictData;
		bDictLoaded = FFileHelper::LoadFileToArray(DictData,*DictDir);
		if(bDictLoaded)
		{
			UE_LOG(LogZstdShader,Display,TEXT("zstd compressor dict loaded!!"));
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
	bool bZstdCompressed = false;
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
			}
			CompressedSize = Result;
			bZstdCompressed = true;
		}
	}

	if(!bZstdCompressed)
	{
		UE_LOG(LogZstdShader,Display,TEXT("zstd compress failed,try using %s !"),*DefaultShaderCompressionName.ToString());
		return FCompression::CompressMemory(DefaultShaderCompressionName,CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize);
	}
	return bZstdCompressed;	
}

bool FZstdShaderCompressionFormat::Uncompress(void* UncompressedBuffer, int32& UncompressedSize,
	const void* CompressedBuffer, int32 CompressedSize, int32 CompressionData)
{
	SCOPED_NAMED_EVENT_TEXT("FZstdShaderCompressionFormat::Uncompress",FColor::Red);
	bool bZstdCompressed = false;
	if(IsDictLoaded())
	{
		auto Result = ZSTD_decompressDCtx(DDict, UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
		if (!ZSTD_isError(Result))
		{
			UncompressedSize = Result;
			bZstdCompressed = true;
		}
	}
	if(!bZstdCompressed)
	{
		UE_LOG(LogZstdShader,Display,TEXT("zstd uncompress failed,try using %s !"),*DefaultShaderCompressionName.ToString());
		return FCompression::UncompressMemory(DefaultShaderCompressionName,UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
	}
	return bZstdCompressed;
}