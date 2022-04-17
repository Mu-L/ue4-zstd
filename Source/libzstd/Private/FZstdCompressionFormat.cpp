#include "FZstdCompressionFormat.h"
#include "zstd.h"

FName FZstdCompressionFormat::GetCompressionFormatName()
{
	return TEXT("zstd");
}

bool FZstdCompressionFormat::Compress(void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize, int32 CompressionData)
{
	// UE_LOG(LogTemp, Log, TEXT("FZstdCompressionFormat::Compress level is %d"), FZstdCompressionFormat::Level);
	int32 Result = ZSTD_compress(CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize, FZstdCompressionFormat::Level);
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
	return false;
}
bool FZstdCompressionFormat::Uncompress(void* UncompressedBuffer, int32& UncompressedSize, const void* CompressedBuffer, int32 CompressedSize, int32 CompressionData)
{
	int32 Result = ZSTD_decompress(UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
	if (Result > 0)
	{
		UncompressedSize = Result;
		return true;
	}
	return false;
}

int32 FZstdCompressionFormat::GetCompressedBufferSize(int32 UncompressedSize, int32 CompressionData)
{
	return ZSTD_compressBound(UncompressedSize);
}

uint32 FZstdCompressionFormat::GetVersion()
{
	return ZSTD_VERSION_NUMBER;
}

FString FZstdCompressionFormat::GetDDCKeySuffix()
{
	static FString DDCKey = FString::Printf(TEXT("ZSTD_%d"),GetVersion());
	return DDCKey;
}
