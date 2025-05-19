#pragma once

#include "TextureImportUI.h"

//D:\UE_5.4\Engine\Source\Editor\UnrealEd\Private\FbxOptionWindow.h
//D:\UE_5.4\Engine\Source\Editor\UnrealEd\Private\Fbx\FbxMainImport.cpp line-238
class STextureOptionWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( STextureOptionWindow )
		: _ImportUIArg(NULL)
		, _WidgetWindow()
		, _FullPath()
		//, _ForcedImportType()
		//, _IsObjFormat(false)
		, _MaxWindowHeight(0.0f)
		, _MaxWindowWidth(0.0f)
	{}

	SLATE_ARGUMENT( UTextureImportUI*, ImportUIArg )
	SLATE_ARGUMENT( TSharedPtr<SWindow>, WidgetWindow )
	SLATE_ARGUMENT( FText, FullPath )
	//SLATE_ARGUMENT( TOptional<EFBXImportType>, ForcedImportType )
	//SLATE_ARGUMENT( bool, IsObjFormat )
	SLATE_ARGUMENT( float, MaxWindowHeight)
	SLATE_ARGUMENT(float, MaxWindowWidth)
SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	virtual bool SupportsKeyboardFocus() const override { return true; }

	FReply OnImport()
	{
		bShouldImport = true;
		if(ImportUI->bSaveAsDefault && ImportUI->GetClass()->HasAnyClassFlags(CLASS_ProjectUserConfig))
		{
			ImportUI->UpdateProjectUserConfigFile();
		}
		if ( WidgetWindow.IsValid() )
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	FReply OnImportAll()
	{
		bShouldImportAll = true;
		return OnImport();
	}

	FReply OnCancel()
	{
		bShouldImport = false;
		bShouldImportAll = false;
		if ( WidgetWindow.IsValid() )
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override
	{
		if( InKeyEvent.GetKey() == EKeys::Escape )
		{
			return OnCancel();
		}

		return FReply::Unhandled();
	}

	bool ShouldImport() const
	{
		return bShouldImport;
	}

	bool ShouldImportAll() const
	{
		return bShouldImportAll;
	}

	STextureOptionWindow() 
		: ImportUI(NULL)
		, bShouldImport(false)
		, bShouldImportAll(false)
	{}
		
private:
	EActiveTimerReturnType SetFocusPostConstruct(double InCurrentTime, float InDeltaTime);
	// bool CanImport() const;
	FReply OnResetToDefaultClick() const;
	FText GetImportTypeDisplayText() const;

private:
	UTextureImportUI*	ImportUI;
	TSharedPtr<class IDetailsView> DetailsView;
	TWeakPtr< SWindow > WidgetWindow;
	TSharedPtr<SButton> ImportAllButton;
	bool			bShouldImport;
	bool			bShouldImportAll;
};
