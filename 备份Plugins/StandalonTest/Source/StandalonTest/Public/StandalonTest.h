// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMenuBuilder;

class FStandalonTestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	
	void AddMenuExtension(FMenuBuilder& Builder);

	void ExecuteImportImage();
	FString CachedSavePath;
	//FString CachedSelectPath = FPaths::ProjectContentDir();
	bool IsPathValid();

	void ReplaceReference();
};
