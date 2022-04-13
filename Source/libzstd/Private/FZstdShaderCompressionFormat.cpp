#include "FZstdShaderCompressionFormat.h"

#include "zstdmt_compress.h"
#include "Misc/Paths.h"
FZstdShaderCompressionFormat::FZstdShaderCompressionFormat(const FString& InDictDir):DictDir(InDictDir)
{
	if(FPaths::FileExists(InDictDir))
	{
		TArray<uint8> DictData;
		bDictLoaded = FFileHelper::LoadFileToArray(DictData,*DictDir);
		if(bDictLoaded)
		{
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
	if(IsDictLoaded())
	{
		int32 Result = ZSTD_compressCCtx(CDict,CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize,
												CompressionData);
		if (Result > 0)
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
		
		return FZstdCompressionFormat::Compress(CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize,
												CompressionData);
	}
	return false;	
}

bool FZstdShaderCompressionFormat::Uncompress(void* UncompressedBuffer, int32& UncompressedSize,
	const void* CompressedBuffer, int32 CompressedSize, int32 CompressionData)
{
	if(IsDictLoaded())
	{
		int32 Result = ZSTD_decompressDCtx(DDict,UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
		if (Result > 0)
		{
			UncompressedSize = Result;
			return true;
		}
		return true;	
	}else{
		return FZstdCompressionFormat::Uncompress(UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize,
												  CompressionData);
	}
}