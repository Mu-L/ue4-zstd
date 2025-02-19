// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/ICompressionFormat.h"
#include "FZstdCompressionFormat.h"

class FlibzstdModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TMap<FName,FZstdCompressionFormat*> ZstdCompressionFormats;
};
