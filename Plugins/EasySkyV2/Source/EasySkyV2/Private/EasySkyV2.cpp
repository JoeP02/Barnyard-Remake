// EasySkyV2, Copyright 2021 PS Studios

#include "EasySkyV2.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FEasySkyV2Module"

void FEasySkyV2Module::StartupModule()
{
	FString PluginName = "EasySkyV2";
	//FString BaseDir = IPluginManager::Get().FindPlugin("AwesomePlugin")->GetBaseDir();
	FString Content = IPluginManager::Get().FindPlugin("EasySkyV2")->GetContentDir();
	FString ShaderDir = FPaths::Combine(Content, TEXT("Shaders"));
	AddShaderSourceDirectoryMapping("/Plugin/Shaders", ShaderDir);
}

void FEasySkyV2Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasySkyV2Module, EasySkyV2)