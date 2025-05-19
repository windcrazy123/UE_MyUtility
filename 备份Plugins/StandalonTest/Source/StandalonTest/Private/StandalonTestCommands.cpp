// Copyright Epic Games, Inc. All Rights Reserved.

#include "StandalonTestCommands.h"

#define LOCTEXT_NAMESPACE "FStandalonTestModule"

void FStandalonTestCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "FriendlyNameTest", "Bring up StandalonTest window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(OpenImportWindow, "MyImportWindow", "Open MY Import Window to Import Texture", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(OpenReplaceReferencesWindow, "ReplaceRef", "Open MY Replace References Window to Consolidate Other Same Textures", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
