// Copyright Andrii Loborchuk. All Rights Reserved. (2024)

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBP_MultithreadingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
