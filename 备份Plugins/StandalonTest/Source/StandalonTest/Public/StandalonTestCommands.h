// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "StandalonTestStyle.h"

class FStandalonTestCommands : public TCommands<FStandalonTestCommands>
{
public:

	FStandalonTestCommands()
		: TCommands<FStandalonTestCommands>(TEXT("StandalonCommandsTest"), NSLOCTEXT("Contexts", "StandalonTest", "StandalonTest Plugin"), NAME_None, FStandalonTestStyle::GetStyleSetName())
	{//绑定到FStandalonTestStyle，在FStandalonTestStyle::Create()函数中可通过InContextName获取到下面的FUICommandInfo来指定缩略图
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
	TSharedPtr< FUICommandInfo > OpenImportWindow;
	TSharedPtr< FUICommandInfo > OpenReplaceReferencesWindow;
};