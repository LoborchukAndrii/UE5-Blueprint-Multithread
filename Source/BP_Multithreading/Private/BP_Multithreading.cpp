// Copyright Andrii Loborchuk. All Rights Reserved. (2024)

#include "BP_Multithreading.h"

#define LOCTEXT_NAMESPACE "FBP_MultithreadingModule"

void FBP_MultithreadingModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBP_MultithreadingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBP_MultithreadingModule, BP_Multithreading)