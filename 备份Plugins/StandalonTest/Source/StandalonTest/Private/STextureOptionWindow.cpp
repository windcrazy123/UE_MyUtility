#include "STextureOptionWindow.h"

#include "SPrimaryButton.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "STextureOptionWindow"

void STextureOptionWindow::Construct(const FArguments& InArgs)
{
	ImportUI = InArgs._ImportUIArg;
	WidgetWindow = InArgs._WidgetWindow;

	check (ImportUI);
	
	TSharedPtr<SBox> ImportTypeDisplay;
	TSharedPtr<SHorizontalBox> FbxHeaderButtons;
	TSharedPtr<SBox> InspectorBox;
	this->ChildSlot
	[
		SNew(SBox)
		.MaxDesiredHeight(InArgs._MaxWindowHeight)
		.MaxDesiredWidth(InArgs._MaxWindowWidth)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SAssignNew(ImportTypeDisplay, SBox)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SBorder)
				.Padding(FMargin(3))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Import_CurrentFileTitle", "Current Asset: "))
					]
					+SHorizontalBox::Slot()
					.Padding(5, 0, 0, 0)
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(InArgs._FullPath)
						.ToolTipText(InArgs._FullPath)
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SAssignNew(InspectorBox, SBox)
				.MaxDesiredHeight(650.0f)
				.WidthOverride(400.0f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(2.0f)
				+ SUniformGridPanel::Slot(1, 0)
				[
					SAssignNew(ImportAllButton, SPrimaryButton)
					.Text(LOCTEXT("TextureOptionWindow_ImportAll", "Import All"))
					.ToolTipText(LOCTEXT("TextureOptionWindow_ImportAll_ToolTip", "Import all files with these same settings"))
					.OnClicked(this, &STextureOptionWindow::OnImportAll)
				]
				+ SUniformGridPanel::Slot(2, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("TextureOptionWindow_Import", "Import"))
					.OnClicked(this, &STextureOptionWindow::OnImport)
				]
				+ SUniformGridPanel::Slot(3, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("TextureOptionWindow_Cancel", "Cancel"))
					.ToolTipText(LOCTEXT("TextureOptionWindow_Cancel_ToolTip", "Cancels importing this FBX file"))
					.OnClicked(this, &STextureOptionWindow::OnCancel)
				]
			]
		]
	];

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	InspectorBox->SetContent(DetailsView->AsShared());

	ImportTypeDisplay->SetContent(
		SNew(SBorder)
		.Padding(FMargin(3))
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &STextureOptionWindow::GetImportTypeDisplayText)
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SAssignNew(FbxHeaderButtons, SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(FMargin(2.0f, 0.0f))
				[
					SNew(SButton)
					.Text(LOCTEXT("TextureOptionWindow_ResetOptions", "Reset to Default"))
					.OnClicked(this, &STextureOptionWindow::OnResetToDefaultClick)
				]
			]
		]
	);

	DetailsView->SetObject(ImportUI);

	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &STextureOptionWindow::SetFocusPostConstruct));
}

EActiveTimerReturnType STextureOptionWindow::SetFocusPostConstruct(double InCurrentTime, float InDeltaTime)
{
	if (ImportAllButton.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(ImportAllButton, EFocusCause::SetDirectly);
	}

	return EActiveTimerReturnType::Stop;
}

FReply STextureOptionWindow::OnResetToDefaultClick() const
{
	ImportUI->ResetToDefault();
	//Refresh the view to make sure the custom UI are updating correctly
	DetailsView->SetObject(ImportUI, true);
	return FReply::Handled();
}

FText STextureOptionWindow::GetImportTypeDisplayText() const
{
	return FText::FromString("Import Image");
}

#undef LOCTEXT_NAMESPACE