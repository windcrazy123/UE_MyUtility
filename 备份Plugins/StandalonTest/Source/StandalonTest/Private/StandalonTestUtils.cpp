#include "StandalonTestUtils.h"

//#include "AssetImportTask.h"
//#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
//#include "IContentBrowserSingleton.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "ImageCore.h"
#include "ImageUtils.h"
#include "STextureOptionWindow.h"
#include "TextureImportUI.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/TextureFactory.h"
//#include "Factories/TextureFactory.h"

DEFINE_LOG_CATEGORY(StandalonTest);

FString StandalonTestUtils::CachedSelectPath = FPaths::ProjectContentDir();
//bool StandalonTestUtils::bImportAll = false;

void StandalonTestUtils::ExecuteImportImage(const FString& CachedSavePath)
{
	UE_LOG(LogTemp, Warning, TEXT("ExecuteImportImage"));
	bool bImportAll = false;
	FString AbsolutePath;
	AbsolutePath = FPaths::ConvertRelativePathToFull(CachedSavePath);

	TArray<FString> SelectedFiles;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if(DesktopPlatform)
	{
		DesktopPlatform->OpenFileDialog(
			FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
			"Select Images",
			CachedSelectPath,"",
			"Image files|*.png;*.jpg;*.jpeg;*.bmp",
			EFileDialogFlags::Multiple,
			SelectedFiles
		);
		if(SelectedFiles.Num() == 0) return;
		CachedSelectPath = FPaths::GetPath(SelectedFiles[0]);
	}
	
	{
		UTextureImportUI* ImportUI = NewObject<UTextureImportUI>();
		FScopedSlowTask SlowTask(SelectedFiles.Num(), NSLOCTEXT("StandaloneTestUtils", "ImportImage", "Importing Textures..."));
		SlowTask.MakeDialog(true);
		for (const FString& File : SelectedFiles)
        {
			if(SlowTask.ShouldCancel()) break;
        	if(!bImportAll)
        	{
        		ImportUI->LoadConfig();
    
        		TSharedRef<SWindow> SettingWindow = SNew(SWindow)
        			.Title(NSLOCTEXT("TextureImportUI", "SettingsTitle", "Texture Import Settings"))
        			.ClientSize(FVector2D(600, 400));
    
        		/*FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        		FDetailsViewArgs DetailsViewArgs;
        		DetailsViewArgs.bAllowSearch = true;
        		DetailsViewArgs.bHideSelectionTip = false;
        		TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
        		
        		DetailsView->SetObject(ImportUI.Get());
        		SettingWindow->SetContent(
        			SNew(SVerticalBox)
        			+SVerticalBox::Slot()
        			.FillHeight(.5f)
        			.Padding(2.0f)
        			[
        				DetailsView.ToSharedRef()
        			]
        			+SVerticalBox::Slot()
        			.AutoHeight()
        			.Padding(5.0f).VAlign(VAlign_Bottom).HAlign(HAlign_Right)
        			[
        				SNew(SHorizontalBox)
        				+SHorizontalBox::Slot()
        				.AutoWidth()
        				.Padding(2.0f)
        				[
        					SNew(SButton)
        					.Text(NSLOCTEXT("TextureImportUI", "ImportButton", "Import"))
        					.OnClicked_Lambda([ImportUI, SelectedFiles, AbsolutePath, SettingWindow]()
        					{
        						if(ImportUI->bSaveAsDefault && ImportUI->GetClass()->HasAnyClassFlags(CLASS_ProjectUserConfig))
        						{
        							ImportUI->UpdateProjectUserConfigFile();
        						}
        						DoActualImport(SelectedFiles, AbsolutePath, ImportUI);
        						SettingWindow->RequestDestroyWindow();
        						return FReply::Handled();
        					})
        				]
        				+SHorizontalBox::Slot()
        				.AutoWidth()
        				.Padding(2.0f)
        				[
        					SNew(SButton)
        					.Text(NSLOCTEXT("TextureImportUI", "CancelButton", "Cancel"))
        					.OnClicked_Lambda([SettingWindow]()
        					{
        						SettingWindow->RequestDestroyWindow();
        						return FReply::Handled();
        					})
        				]
        			]
        		);*/
    
        		TSharedPtr<STextureOptionWindow> TextureOptionWindow;
        		SettingWindow->SetContent(
        			SAssignNew(TextureOptionWindow, STextureOptionWindow)
        			.ImportUIArg(ImportUI)
        			.FullPath(FText::FromString(File))
        			.WidgetWindow(SettingWindow)
        			.MaxWindowHeight(600.f)
        			.MaxWindowWidth(450.f)
        		);
        		
        		FSlateApplication::Get().AddModalWindow(SettingWindow, FGlobalTabmanager::Get()->GetRootWindow());
        		
        		bImportAll = TextureOptionWindow->ShouldImportAll();
        		if(!TextureOptionWindow->ShouldImport()) break;
        	}
        	DoActualImport(File, AbsolutePath, ImportUI);
			SlowTask.EnterProgressFrame(1.f, FText::Format(NSLOCTEXT("StandaloneTestUtils", "ImportedImage", "Importing {0}..."), FText::FromString(File)));
        }
	}
}

//AssetToolsModule.Get().ImportAssetTasks(ImportTasks);在源码中貌似将Task的Factory中的Property给忽略掉了，因此我需要实现的导入函数
/*void StandalonTestUtils::DoActualImport(const TArray<FString>& Files, const FString& TargetPath,
	UTextureImportUI* Settings)
{
	UE_LOG(LogTemp, Warning, TEXT("DoActualImport"));
	UE_LOG(LogTemp, Warning, TEXT("targetpath%s\nsourcepath: "), *TargetPath);
	for(const auto& File : Files)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *File);
	}
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	
	TArray<UAssetImportTask*> ImportTasks;
	for(const FString& File : Files)
	{
		UAssetImportTask* ImportTask = NewObject<UAssetImportTask>();
		ImportTask->Filename = File;
		ImportTask->DestinationPath = TargetPath;
		ImportTask->bSave = false;
		ImportTask->bReplaceExisting = true;
		ImportTask->bAutomated = false;
		ImportTask->bAsync = false;
		//ImportTask->bReplaceExisting = Settings->bReplaceExisting;

		UTextureFactory* TextureFactory = NewObject<UTextureFactory>();
		TextureFactory->AddToRoot();
		ApplyImportSettingToFactory(TextureFactory, Settings);
		TextureFactory->PostInitProperties();

		ImportTask->Factory = TextureFactory;
		ImportTasks.Add(ImportTask);
	}
	
	AssetToolsModule.Get().ImportAssetTasks(ImportTasks);

	TArray<UObject*> ImportedAssets;
	for(auto Task : ImportTasks)
	{
		if(Task->ImportedObjectPaths.Num() > 0)
		{
			if (UObject* Obj = LoadObject<UObject>(nullptr, *Task->ImportedObjectPaths[0]))
			{
				ImportedAssets.Add(Obj);
				if(UTexture2D* ImportedTexture = Cast<UTexture2D>(Obj))
				{
					UE_LOG(LogTemp, Warning, TEXT("ImportedTexture: %s"), *ImportedTexture->GetPathName());
					UE_LOG(LogTemp, Warning, TEXT("ImportedTexture: %s"), *ImportedTexture->GetName());
					UE_LOG(LogTemp, Log, TEXT(" - 压缩设置: %d"), (int32)ImportedTexture->CompressionSettings);
					UE_LOG(LogTemp, Log, TEXT(" - 最大尺寸: %d"), ImportedTexture->GetMaximumDimension());
					UE_LOG(LogTemp, Log, TEXT(" - Mip生成: %d"), (int32)ImportedTexture->MipGenSettings);
				}
			}
			for(auto Path : Task->ImportedObjectPaths)
			{
				UE_LOG(LogTemp, Warning, TEXT("ImportedObjectPaths: %s"), *Path);
				/*UObject* Asset = StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
				if(Asset)
				{
					ImportedAssets.Add(Asset);
				}#1#
			}
		}
	}

	if(ImportedAssets.Num() > 0)
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(ImportedAssets);
	}
}
void StandalonTestUtils::ApplyImportSettingToFactory(UTextureFactory* Factory, UTextureImportUI* Settings)
{
	Factory->RemoveFromRoot();
	Factory->SetFlags(RF_Public | RF_Standalone);
	
	//Factory->SuppressImportOverwriteDialog();
	Factory->CompressionSettings = Settings->CompressionSettings;
	Factory->MipGenSettings = Settings->MipGenSettings;
	Factory->LODGroup = Settings->TextureGroup;

	Factory->ConfigureProperties();
}*/

void StandalonTestUtils::DoActualImport(const FString& File, const FString& TargetPath,
	UTextureImportUI* Settings)
{
	UE_LOG(LogTemp, Warning, TEXT("DoActualImport"));
	UE_LOG(LogTemp, Warning, TEXT("targetpath%s\nsourcepath: "), *TargetPath);
	/*for(const auto& File : Files)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *File);
	}*/
	
	//for(const FString& File : Files)
	{
		// TArray<uint8> SourceImageData;
		// if(!FFileHelper::LoadFileToArray(SourceImageData, *File))
		// {
		// 	UE_LOG(StandalonTest, Error, TEXT("%s,Failed to load file"),*File);
		// 	//continue;
		// 	return;
		// }
		
		/*ImageWrapper->GetRawImage(RawImage);无法调用因为没有导出且不是虚函数
		  TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapper(SourceImageData);
		  if(!ImageWrapper.IsValid())
		  {
		  	UE_LOG(StandalonTest, Error, TEXT("%s,Failed to Get image wrapper"),*File);
		  	//continue;
		  	return;
		  }
		 if(ImageWrapper->SetCompressed(SourceImageData.GetData(),SourceImageData.Num()))
		 {
		 	ImageWrapper->GetRawImage(RawImage);
		 }*/

		FImage RawImage;
		FImageUtils::LoadImage(*File, RawImage);//也是先LoadFileToArray然后DecompressImage
		
		//FImageUtils::DecompressImage(SourceImageData.GetData(), SourceImageData.Num(), RawImage);

		FImage DestSizeImage;
		//RawImage.ResizeTo()函数在InitImageStorage(TempDestImage);有安全检查，所以GammaSpace不是Linear的时候Format应该使用BGRA8，不知道这个和sRGB有没有关系
		//而且使用Texture到Texture也应该将sRGB设为false；https://zhuanlan.zhihu.com/p/66890786
		if(Settings->bTextureResize)
		{
			RawImage.ResizeTo(DestSizeImage,Settings->TextureSize.X,Settings->TextureSize.Y,ERawImageFormat::Type::BGRA8,RawImage.GetGammaSpace());
		}else
		{
			// RawImage.ResizeTo(DestSizeImage,RawImage.SizeX,RawImage.SizeY,ERawImageFormat::Type::BGRA8,EGammaSpace::Linear);
			DestSizeImage = RawImage;
			UE_LOG(StandalonTest, Log, TEXT("No resize, X:%d,Y:%d"),RawImage.SizeX,RawImage.SizeY);
		}
		
		//FImageCore::ResizeImage(FImageView())
		if(!DestSizeImage.IsImageInfoValid())
		{
			UE_LOG(StandalonTest, Error, TEXT("%s,Failed to resize image"),*File);
			//continue;
			return;
		}

		TArray<FColor> Pixels;
		Pixels.Empty();
		const int64 PixelCount = DestSizeImage.SizeX * DestSizeImage.SizeY;
		UE_LOG(StandalonTest, Log, TEXT("ResizeImage: %d,%d"),DestSizeImage.SizeX,DestSizeImage.SizeY);
		const uint8* RawDataPtr = DestSizeImage.RawData.GetData();

		Pixels.Reserve(PixelCount);
		for (int64 i = 0; i < PixelCount; ++i)
		{
			const int64 Offset = i * 4; // BGRA8
			Pixels.Add(FColor(
				RawDataPtr[Offset + 2],  // R
				RawDataPtr[Offset + 1],  // G
				RawDataPtr[Offset + 0],  // B
				RawDataPtr[Offset + 3]   // A
			));
		}

		if(Pixels.IsEmpty())
		{
			UE_LOG(StandalonTest, Error, TEXT("Failed to convert FImage to pixels"));
			//continue;
			return;
		}

		//纹理参数
		FCreateTexture2DParameters Params;
		ApplyImportSettingToFactory(Params, Settings);

		FString NewTexName = FPaths::GetBaseFilename(File);
		FString NewTexPath = TargetPath + "/";
		FString NewTexPackageName = NewTexPath + NewTexName;

		UPackage* NewTexturePackage = FindPackage(nullptr, *NewTexPackageName);
		if(!NewTexturePackage)
		{
			UE_LOG(StandalonTest, Log, TEXT(" - 创建新包"))
			NewTexturePackage = CreatePackage(*NewTexPackageName);
		}
		
		if(!NewTexturePackage)
		{
			UE_LOG(StandalonTest, Error, TEXT("Failed to create package"));
			//continue;
			return;
		}
		
		//创建新纹理
		UTexture2D* NewTexture = FImageUtils::CreateTexture2D(
			DestSizeImage.SizeX, DestSizeImage.SizeY,
			Pixels,
			NewTexturePackage,
			NewTexName,
			RF_Public | RF_Standalone,
			Params
		);
		if(!NewTexture)
		{
			UE_LOG(StandalonTest, Error, TEXT("Failed to create texture"));
			//continue;
			return;
		}

		FAssetRegistryModule::AssetCreated(NewTexture);
		if(!NewTexturePackage->MarkPackageDirty()) return;
			//continue;
	}
}

TSharedPtr<IImageWrapper> StandalonTestUtils::GetImageWrapper(TArray<uint8>& SourceImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(SourceImageData.GetData(), SourceImageData.GetAllocatedSize());
	
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if(!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create image wrapper"))
		return nullptr;
	}

	if(!ImageWrapper->SetCompressed(SourceImageData.GetData(), SourceImageData.GetAllocatedSize()))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to set compressed image data"))
		return nullptr;
	}
	return ImageWrapper;
}

void StandalonTestUtils::ApplyImportSettingToFactory(FCreateTexture2DParameters& Params, const UTextureImportUI* Settings)
{
	Params.bUseAlpha = true;
	Params.CompressionSettings = Settings->CompressionSettings;
	Params.TextureGroup = Settings->TextureGroup;
	Params.MipGenSettings = Settings->MipGenSettings;
}

bool StandalonTestUtils::IsPathValid(const FString& CachedSavePath)
{
	return !CachedSavePath.IsEmpty();
}

void StandalonTestUtils::ReplaceReferences()
{
	if(EAppReturnType::Ok != FMessageDialog::Open(EAppMsgType::Type::OkCancel, FText::FromString("Are you sure you want to use one asset to consolidate the same name assets to?\nThis operation is irreversible and it is recommended to back up the project before running it.")))
		return;

	// 1. 获取所有资产并按名称分组
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> AllAssets;
	FARFilter Filter;
	Filter.ClassPaths.Add(UTexture2D::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(TEXT("/Game/"));
	Filter.bRecursivePaths = true;
	AssetRegistryModule.Get().GetAssets(Filter, AllAssets);

	// 按名称分组
	TMap<FName, TArray<FAssetData>> NameToAssetsMap;
	for (const FAssetData& Asset : AllAssets)
	{
		NameToAssetsMap.FindOrAdd(Asset.AssetName).Add(Asset);
	}

	// 2. 过滤出有重复名称的组
	TArray<TArray<FAssetData>> DuplicateGroups;
	for (const auto& Pair : NameToAssetsMap)
	{
		if (Pair.Value.Num() > 1)
		{
			DuplicateGroups.Add(Pair.Value);
		}
	}

	// 4. 替换引用为每个组的第一个资产
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	for (TArray<FAssetData>& Group : DuplicateGroups)
	{
		if (Group.Num() < 2) continue;

		
		for (int32 i = 1; i < Group.Num(); ++i)
		{
			const FAssetData& PrimaryAsset = Group[0];
			
			const FAssetData& DuplicateAsset = Group[i];
			auto Object = DuplicateAsset.GetAsset();
			FString NewPackagePath = PrimaryAsset.PackagePath.ToString();
			FString NewName = PrimaryAsset.AssetName.ToString();
            
			// 构建重命名参数
			FAssetRenameData RenameData(
				Object,
				NewPackagePath,
				NewName
			);

			
			TArray<FAssetRenameData> RenameDataList;
			RenameDataList.Add(RenameData);
			// 执行重命名（自动生成重定向器）
			AssetTools.RenameAssets(RenameDataList);
		}
		
	}
}



