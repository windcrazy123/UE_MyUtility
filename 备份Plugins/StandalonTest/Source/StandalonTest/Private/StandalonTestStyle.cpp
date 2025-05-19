// Copyright Epic Games, Inc. All Rights Reserved.

#include "StandalonTestStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FStandalonTestStyle::StyleInstance = nullptr;

void FStandalonTestStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FStandalonTestStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FStandalonTestStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("StandalonTestStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FStandalonTestStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("StandalonTestStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("StandalonTest")->GetBaseDir() / TEXT("Resources"));

	Style->Set("StandalonCommandsTest.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	Style->Set("StandalonCommandsTest.OpenImportWindow", new IMAGE_BRUSH(TEXT("Icon128"), Icon16x16));
	Style->Set("StandalonCommandsTest.OpenReplaceReferencesWindow", new IMAGE_BRUSH(TEXT("Icon128"), Icon16x16));

	return Style;
}

void FStandalonTestStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FStandalonTestStyle::Get()
{
	return *StyleInstance;
}
