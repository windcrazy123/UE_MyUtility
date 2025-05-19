// Copyright Epic Games, Inc. All Rights Reserved.

#include "StandalonTest.h"

#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "StandalonTestStyle.h"
#include "StandalonTestCommands.h"
#include "LevelEditor.h"
#include "StandalonTestUtils.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"


static const FName StandalonTestTabName("StandalonTest");

#define LOCTEXT_NAMESPACE "FStandalonTestModule"

void FStandalonTestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FStandalonTestStyle::Initialize();
	FStandalonTestStyle::ReloadTextures();

	FStandalonTestCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	//Replace References
	PluginCommands->MapAction(
		FStandalonTestCommands::Get().OpenReplaceReferencesWindow,
		FExecuteAction::CreateRaw(this, &FStandalonTestModule::ReplaceReference),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FStandalonTestCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FStandalonTestModule::PluginButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FStandalonTestCommands::Get().OpenImportWindow,
		FExecuteAction::CreateRaw(this, &FStandalonTestModule::ExecuteImportImage),
		FCanExecuteAction::CreateRaw(this, &FStandalonTestModule::IsPathValid));

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FStandalonTestModule::RegisterMenus));
	
	//InvokeTab called
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(StandalonTestTabName, FOnSpawnTab::CreateRaw(this, &FStandalonTestModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FStandalonTestTabTitle", "StandalonTest"))
		.SetMenuType(ETabSpawnerMenuType::Disabled)//在菜单中显示，默认为显示
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory());//Windows->LevelEditor下面的菜单栏出现,不加这个的话会显示在好多地方这个是限定位置

	
	//---===---
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	auto& ACMExtenders = ContentBrowserModule.GetAllAssetContextMenuExtenders();
	ACMExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateLambda([this](const TArray<FString>& Path)
	{
		if(Path.Num() > 0)
		{
			CachedSavePath = Path[0];
			UE_LOG(LogTemp, Warning, TEXT("SelectedPath: %s"), *CachedSavePath);
		}
		TSharedRef<FExtender> Extender = MakeShared<FExtender>();
		Extender->AddMenuExtension(
			"ContentBrowserNewAdvancedAsset",
			EExtensionHook::After,
			PluginCommands,
			FMenuExtensionDelegate::CreateRaw(this, &FStandalonTestModule::AddMenuExtension)
		);
		return Extender;
	}));
}
void FStandalonTestModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.BeginSection("MyContentSection", LOCTEXT("MySection", "My Section"));
	Builder.AddMenuEntry(FStandalonTestCommands::Get().OpenImportWindow,"BuilderSection",FText::FromString("BuilderTest"),LOCTEXT("BuilderTestSection", "Builder Test Section"));
	Builder.EndSection();
}
void FStandalonTestModule::ExecuteImportImage()
{
	StandalonTestUtils::ExecuteImportImage(CachedSavePath);
}
bool FStandalonTestModule::IsPathValid()
{
	return StandalonTestUtils::IsPathValid(CachedSavePath);
}

void FStandalonTestModule::ReplaceReference()
{
	StandalonTestUtils::ReplaceReferences();
}

void FStandalonTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FStandalonTestStyle::Shutdown();

	FStandalonTestCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(StandalonTestTabName);
}

TSharedRef<SDockTab> FStandalonTestModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FStandalonTestModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("StandalonTest.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FStandalonTestModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(StandalonTestTabName);
}

void FStandalonTestModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FStandalonTestCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FStandalonTestCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.Toolbar");
		FToolMenuEntry& Entry = Menu->FindOrAddSection("New").AddEntry(FToolMenuEntry::InitToolBarButton(FStandalonTestCommands::Get().OpenReplaceReferencesWindow));
		Entry.SetCommandList(PluginCommands);
	}

	/*{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AddNewContextMenu");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("ContentBrowserGetContent");
			Section.AddMenuEntry(
				"MyImportAsset",
				FText::Format(LOCTEXT("ImportAsset", "Import to {0}..."), FText::FromName(FirstSelectedPath)),
				LOCTEXT("ImportAssetTooltip_NewAsset", "Imports an asset from file to this folder."),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Import"),
				FUIAction(
					FExecuteAction::CreateStatic(&FNewAssetContextMenu::ExecuteImportAsset, InOnImportAssetRequested, FirstSelectedPath),
					CanExecuteAssetActionsDelegate
					)
				).InsertPosition = FToolMenuInsert(NAME_None, EToolMenuInsertType::First);
		}
	}*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStandalonTestModule, StandalonTest)