#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/ICompressionFormat.h"
#include "zstd.h"

struct LIBZSTD_API FZstdCompressionFormat : public ICompressionFormat
{
	virtual FName GetCompressionFormatName()override;
	virtual ~FZstdCompressionFormat(){}
	virtual bool Compress(void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize, int32 CompressionData)override;
	virtual bool Uncompress(void* UncompressedBuffer, int32& UncompressedSize, const void* CompressedBuffer, int32 CompressedSize, int32 CompressionData)override;
	virtual int32 GetCompressedBufferSize(int32 UncompressedSize, int32 CompressionData)override;
	virtual uint32 GetVersion();
	virtual FString GetDDCKeySuffix();
	static int32 Level;
};