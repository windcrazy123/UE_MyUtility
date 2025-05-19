// Fill out your copyright notice in the Description page of Project Settings.


#include "tes_aoteman/Public/BFL_Utility.h"

#include "AssetToolsModule.h"
//#include "IAssetTools.h"
#include "EditorAssetLibrary.h"
#include "EditorDirectories.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "AssetSelection.h"
#include "TextureAssetActions.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/TextureFactory.h"
#include "Misc/AsciiSet.h"

DEFINE_LOG_CATEGORY(BFLUtilityLog);

float UBFL_Utility::GetCurrentTime()
{
	return FPlatformTime::Seconds();
}
void UBFL_Utility::ResizeTest(const TArray<UTexture*>& InTextures, int32 InMaxSize)
{
	//方法一
	//UE::TextureAssetActions::TextureSource_Resize_WithDialog(InTextures);
	
	//方法二
	/*static const FName MaxTextureSizeName = GET_MEMBER_NAME_CHECKED(UTexture, MaxTextureSize);
	FProperty* MaxTextureSizeProperty = FindFProperty<FProperty>( UTexture::StaticClass(), MaxTextureSizeName );
	FPropertyChangedEvent PropertyChangedEvent(MaxTextureSizeProperty);

	for (UTexture2D* Texture : InTextures)
	{
		Texture->PreEditChange(MaxTextureSizeProperty);

		const int32 TextureWidth = Texture->GetSizeX();
		const int32 TextureHeight = Texture->GetSizeY();
		if (!FMath::IsPowerOfTwo(TextureWidth) || !FMath::IsPowerOfTwo(TextureHeight))
		{
			// Need to specify power of two mode for non-pot textures
			Texture->PowerOfTwoMode = ETexturePowerOfTwoSetting::PadToPowerOfTwo;
		}
		Texture->MaxTextureSize = InMaxSize;
		Texture->PostEditChangeProperty(PropertyChangedEvent);
	}*/

	int BuildSizeX;
	int BuildSizeY;
	// we do the resizing considering only mip/LOD/build settings for the running Editor platform (eg. Windows)
	const ITargetPlatform* RunningPlatform = GetTargetPlatformManagerRef().GetRunningTargetPlatform();
	for (UTexture* Texture : InTextures)
	{
		Texture->GetBuiltTextureSize(RunningPlatform, BuildSizeX, BuildSizeY);
		UE_LOG(BFLUtilityLog, Warning, TEXT("%d,%d"),BuildSizeX,BuildSizeY);
	}
}

void UBFL_Utility::ReplaceRefTest(UTexture2D* Texture, UTexture2D* OtherTexture)
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	FString NewPackagePath = OtherTexture->GetPackage()->GetName();//.Replace(*OtherTexture->GetName(), TEXT(""));
	FString NewName = OtherTexture->GetName();
	FAssetRenameData ReplaceData(Texture, NewPackagePath, NewName);
	TArray<FAssetRenameData> AssetsToRename;
	AssetsToRename.Add(ReplaceData);
	AssetTools.RenameAssets(AssetsToRename);
	
}

void UBFL_Utility::ImportImage(const TArray<FString>& SourcePaths, const FString& DestinationPath)
{
	constexpr FAsciiSet Delimiters = FAsciiSet(".") + (char)SUBOBJECT_DELIMITER_CHAR;
	FString InOutName = "tdaaa/bbb/ccc/ttt/ddd.tttd";
	const TCHAR* DelimiterOrEnd = FAsciiSet::FindFirstOrEnd(*InOutName, Delimiters);
	bool bt = !!FCString::Strstr(TEXT("aaa/bbb/ccc/ttt/ddd:t"), TEXT("."));
	FString str = DelimiterOrEnd;
	FString s1 = FPackageName::GetShortName(TEXT("aaa/bbb/ccc/ttt/ddd"));
	//const int32 DotIndex = static_cast<int32>(DelimiterOrEnd - *InOutName);
	FString InOutName1 = "tdaaa/bbb/ccc/ttt/ddd.tttd";
	const int32 DotIndex = static_cast<int32>(DelimiterOrEnd - *InOutName1);
	if(str == "\0") UE_LOG(BFLUtilityLog, Error, TEXT("Invalid image path sss%d,%s,dot%d"),bt,*s1,DotIndex)
	UE_LOG(BFLUtilityLog, Warning, TEXT("%s,sss%d,%s,dot%d"),DelimiterOrEnd,bt,*s1,DotIndex)
	
}

TSharedPtr<IImageWrapper> UBFL_Utility::GetImageWrapper(TArray<uint8>& SourceImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(SourceImageData.GetData(), SourceImageData.GetAllocatedSize());
	
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if(!ImageWrapper.IsValid())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to create image wrapper"))
		return nullptr;
	}

	if(!ImageWrapper->SetCompressed(SourceImageData.GetData(), SourceImageData.GetAllocatedSize()))
	{
		// UE_LOG(BFLUtilityLog, Warning, TEXT("Failed to set compressed image data, just has Orginal Image Format"))
		// return ImageWrapper;
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to set compressed image data"))
		return nullptr;
	}
	return ImageWrapper;
	
	/*if (ImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (ImagePath.EndsWith(".jpg") || ImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (ImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (ImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);
	}
	else if (ImagePath.EndsWith(".exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (ImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}
	return nullptr;*/
}

TSharedPtr<IImageWrapper> UBFL_Utility::GetImageWrapper(const FString& ImagePath)
{
	if(!FPaths::FileExists(ImagePath))
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Invalid image path"));
		return nullptr;
	}

	TArray<uint8> SourceImageData;
	if(!FFileHelper::LoadFileToArray(SourceImageData, *ImagePath))
		return nullptr;

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapper(SourceImageData);
	if(!ImageWrapper.IsValid())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to Get image wrapper"))
		return nullptr;
	}
	return ImageWrapper;
}

void UBFL_Utility::ExportTexture(const TArray<FString>& SourcePaths, const FString& DestinationPath)
{
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

	//TArray<FString> SourcePaths = {SourcePath};
	
	AssetTools.ExportAssets(SourcePaths, DestinationPath);
}

void UBFL_Utility::ImportTexture(const TArray<FString>& SourcePaths, const FString& DestinationPath)
{
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	UTextureFactory* TextureFactory = NewObject<UTextureFactory>();
	TextureFactory->SuppressImportOverwriteDialog();

	//TArray<FString> SourceFiles = {SourcePath};
	TArray<UObject*> ImportedAssets = AssetTools.ImportAssets(SourcePaths, DestinationPath, TextureFactory,true);
	
	UEditorAssetLibrary::SaveLoadedAssets(ImportedAssets);
}

void UBFL_Utility::ResizeSelectedTexture_Mip(int32 LODBias)
{
	//FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	{
#define LOCTEXT_NAMESPACE "ResizeTexture"
		FScopedSlowTask SlowTask(SelectedAssets.Num(), LOCTEXT("ResizeSelectedTexture", "Resizing Selected Textures..."));
		SlowTask.MakeDialog(true);
		for (auto Asset : SelectedAssets)
		{
			UObject* Object = Asset.GetAsset();
			if(SlowTask.ShouldCancel())
				break;
			SlowTask.EnterProgressFrame(1.0f, FText::Format(LOCTEXT("ResizingTexture", "Resizing {0}..."), FText::FromName(Asset.PackageName)));
			if (UTexture2D* Texture = Cast<UTexture2D>(Object))
			{
				ResizeTexture(Texture, LODBias);
			}
		}
#undef LOCTEXT_NAMESPACE
	}
}

bool UBFL_Utility::ResizeTexture(UTexture2D* Texture, int32 LODBias)
{
	//if(Texture->GetSizeX() < pow(2,LODBias) || Texture->GetSizeY() < pow(2,LODBias)) return false;
	if(!FMath::IsPowerOfTwo(Texture->GetSizeX())) return false;
	if(!FMath::IsPowerOfTwo(Texture->GetSizeY())) return false;
	
	//cal mip bias
	int32 TargetSizeX = (Texture->GetSizeX() >> LODBias) > 0 ? (Texture->GetSizeX() >> LODBias) : 1;
	int32 TargetSizeY = (Texture->GetSizeY() >> LODBias) > 0 ? (Texture->GetSizeY() >> LODBias) : 1;
	if(TargetSizeX == 1 && TargetSizeY == 1)
	{
		LODBias = Texture->Source.GetNumMips()-1;
	}

	FString NewTexName = Texture->GetName()+"_Resized";
	//FString NewTexPath = Texture->GetPathName();
	FString NewTexPath = Texture->GetPackage()->GetName().Replace(*Texture->GetName(),TEXT("")) + "Resized/";
	FString NewTexPackageName = NewTexPath + NewTexName;

	UPackage* NewTexturePackage = CreatePackage(*NewTexPackageName);

	UTexture2D* NewTexture = NewObject<UTexture2D>(NewTexturePackage, FName(*NewTexName), RF_Public | RF_Standalone);

	//uint32 TexWidth = Texture->GetSizeX();
	//uint32 TexHeight = Texture->GetSizeY();
	
	//=========== init size, format； https://blog.csdn.net/qq_29523119/article/details/124030787 ==============
	//NewTexture->Source.Init(TexWidth,TexHeight, 1, 1, Texture->Source.GetFormat());
	NewTexture->Source.Init(TargetSizeX,TargetSizeY, 1, 1, Texture->Source.GetFormat());
	
	//Copy
	//TArray64<uint8> MipData;
	Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	Texture->SRGB = false;  //确保这些设置，否则颜色容易丢失
	//VideoTextureColor->LODGroup = TEXTUREGROUP_Pixels2D;
	Texture->UpdateResource();
	TIndirectArray<FTexture2DMipMap> Mips = Texture->GetPlatformMips();
	FBulkData TextureBulkData = Mips[LODBias].BulkData;
	uint8* MipData = (uint8*)TextureBulkData.LockReadOnly();
	if (!MipData)
	{
		UE_LOG(BFLUtilityLog, Warning, TEXT("Mip %d data not initialized for texture %s"), LODBias, *Texture->GetName());
		TextureBulkData.Unlock();
		return false;
	}
   
	uint8* NewMipData = NewTexture->Source.LockMip(0);
	if(!NewMipData) return false;
	//uint8* TexMipData = Texture->Source.LockMip(LODBias);
	//if(!TexMipData) return false;
	FMemory::Memcpy(NewMipData, MipData, TargetSizeX* TargetSizeY * sizeof(uint8) * 4);//4个通道的值
	NewTexture->Source.UnlockMip(0);
	TextureBulkData.Unlock();
	//Texture->Source.UnlockMip(LODBias);

	NewTexture->SRGB = Texture->SRGB;
	NewTexture->CompressionSettings = Texture->CompressionSettings;
	NewTexture->MipGenSettings = Texture->MipGenSettings;
	NewTexture->CompressionNoAlpha = Texture->CompressionNoAlpha;
	NewTexture->CompressionQuality = Texture->CompressionQuality;
	NewTexture->LODBias = Texture->LODBias;
	NewTexture->LODGroup = Texture->LODGroup;
	NewTexture->PostEditChange();

	FAssetRegistryModule::AssetCreated(NewTexture);
	if(!NewTexturePackage->MarkPackageDirty())
		return false;

	return true;
}


void UBFL_Utility::ResizeSelectedTexture_Replace(int32 TargetSizeX, int32 TargetSizeY, bool bUseMaxInGame)
{
	TArray<FAssetData> SelectedAssets;
	//GEditor->GetSelectedObjects()->GetSelectedObjects(UTexture2D::StaticClass(),SelectedObjects);
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	{
#define LOCTEXT_NAMESPACE "ResizeTexture"
		FScopedSlowTask SlowTask(SelectedAssets.Num(), LOCTEXT("ResizeSelectedTexture", "Resizing Selected Textures..."));
		SlowTask.MakeDialog(true);
		
		// we do the resizing considering only mip/LOD/build settings for the running Editor platform (eg. Windows)
		const ITargetPlatform* RunningPlatform = GetTargetPlatformManagerRef().GetRunningTargetPlatform();
		
		for (auto Asset : SelectedAssets)
		{
			UObject* Object = Asset.GetAsset();
			if(SlowTask.ShouldCancel())
				break;
			SlowTask.EnterProgressFrame(1.0f, FText::Format(LOCTEXT("ResizingTexture", "Resizing {0}..."), FText::FromName(Asset.PackageName)));
			if (UTexture2D* Texture = Cast<UTexture2D>(Object))
			{
				if(bUseMaxInGame)
				{
					Texture->GetBuiltTextureSize(RunningPlatform, TargetSizeX, TargetSizeY);
				}
				if(Texture->GetSizeX() <= TargetSizeX || Texture->GetSizeY() <= TargetSizeY) continue;
				
				ResizeTexture(Texture, TargetSizeX, TargetSizeY, true);
			}
		}
#undef LOCTEXT_NAMESPACE
	}
}

void UBFL_Utility::ResizeSelectedTexture_Img(int32 TargetSizeX, int32 TargetSizeY)
{
	//FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	{
#define LOCTEXT_NAMESPACE "ResizeTexture"
		TArray<UObject*> NewTextures;
		FScopedSlowTask SlowTask(SelectedAssets.Num(), LOCTEXT("ResizeSelectedTexture", "Resizing Selected Textures..."));
		SlowTask.MakeDialog(true);
		for (auto Asset : SelectedAssets)
		{
			UObject* Object = Asset.GetAsset();
			if(SlowTask.ShouldCancel())
				break;
			SlowTask.EnterProgressFrame(1.0f, FText::Format(LOCTEXT("ResizingTexture", "Resizing {0}..."), FText::FromName(Asset.PackageName)));
			if (UTexture2D* Texture = Cast<UTexture2D>(Object))
			{
				if(UTexture2D* NewTexture = ResizeTexture(Texture, TargetSizeX, TargetSizeY, false))
				{
					NewTextures.Add(NewTexture);
				}
			}
		}
		IAssetTools::Get().SyncBrowserToAssets(NewTextures);//使NewTextures在ContentBrowser中处于选中状态（蓝色高亮）
#undef LOCTEXT_NAMESPACE
	}
}

UTexture2D* UBFL_Utility::ResizeTexture(UTexture2D* Texture, int32 TargetSizeX, int32 TargetSizeY, bool bReplace)
{
	FString NewTexName;
	
	FString NewTexPackageName;
	if(bReplace)
	{
		NewTexName = Texture->GetName();
		NewTexPackageName = Texture->GetPackage()->GetName();
	}else
	{
		NewTexName = Texture->GetName()+"_Resized"+FString::Format(TEXT("{0}_{1}"), {TargetSizeX, TargetSizeY});
		FString NewTexPath = Texture->GetPackage()->GetName().Replace(*Texture->GetName(),TEXT("")) + "Resized/";
		NewTexPackageName = NewTexPath + NewTexName;
	}
	
	UPackage* NewTexturePackage = CreatePackage(*NewTexPackageName);
	if(!NewTexturePackage)
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to create package"));
		return nullptr;
	}

	//获取像素
	TArray<FColor> ResizedPixels;
	if(!GetTexturePixels(ResizedPixels, Texture, TargetSizeX, TargetSizeY))
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to get pixels"));
		return nullptr;
	}
	
	//纹理参数
	FCreateTexture2DParameters Params;
	Params.bUseAlpha = Texture->HasAlphaChannel();
	Params.CompressionSettings = Texture->CompressionSettings;
	Params.TextureGroup = Texture->LODGroup;
	Params.MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

	//创建新纹理
	UTexture2D* NewTexture = FImageUtils::CreateTexture2D(
		TargetSizeX, TargetSizeY,
		ResizedPixels,
		NewTexturePackage,
		NewTexName,
		RF_Public | RF_Standalone,
		Params
	);
	if(!NewTexture)
		return nullptr;

	FAssetRegistryModule::AssetCreated(NewTexture);
	NewTexturePackage->MarkPackageDirty();
	
	NewTexturePackage->SetDirtyFlag(true);

	if(!NewTexturePackage->IsDirty())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to mark package dirty"))
		return nullptr;
	}
	
	return NewTexture;
}

bool UBFL_Utility::GetTexturePixels(TArray<FColor>& OutPixels, UTexture2D* OriginalTexture, int32 DestSizeX , int32 DestSizeY)
{
	// 验证原始纹理
	if (!OriginalTexture || !OriginalTexture->Source.IsValid())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Invalid source texture"));
		return false;
	}

	// 获取原始图像数据
	FImage Image;
	if (!OriginalTexture->Source.GetMipImage(Image, 0))
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to get source image"));
		return false;
	}

	// 转换为标准格式
	if (Image.Format != ERawImageFormat::Type::BGRA8)
	{
		Image.ChangeFormat(ERawImageFormat::Type::BGRA8, Image.GammaSpace);
		if (Image.Format != ERawImageFormat::Type::BGRA8)
		{
			UE_LOG(BFLUtilityLog, Error, TEXT("Failed to convert to BGRA8 format"));
			return false;
		}
	}

	// 调整图像尺寸
	DestSizeX = DestSizeX > 0 ? DestSizeX : Image.SizeX;
	DestSizeY = DestSizeY > 0 ? DestSizeY : Image.SizeY;
	FImage ResizedImage;
	Image.ResizeTo(ResizedImage, DestSizeX , DestSizeY, ERawImageFormat::Type::BGRA8, Image.GammaSpace);
	if (ResizedImage.GetNumPixels()<=0)
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to resize image"));
		return false;
	}

	// 创建颜色数组
	OutPixels.Empty();
	const int64 PixelCount = ResizedImage.SizeX * ResizedImage.SizeY;
	const uint8* RawDataPtr = ResizedImage.RawData.GetData();

	OutPixels.Reserve(PixelCount);
	for (int64 i = 0; i < PixelCount; ++i)
	{
		const int64 Offset = i * 4; // BGRA8每个像素4字节
		OutPixels.Add(FColor(
			RawDataPtr[Offset + 2],  // R
			RawDataPtr[Offset + 1],  // G
			RawDataPtr[Offset + 0],  // B
			RawDataPtr[Offset + 3]   // A
		));
	}

	if(OutPixels.Num()<=0)
		return false;

	return true;
}

bool UBFL_Utility::GetImagePixels(TArray<FColor>& OutPixels, int32& OutSizeX, int32& OutSizeY, const FString& ImagePath)
{
	if(!FPaths::FileExists(ImagePath))
		return false;
	
	TArray<uint8> SourceImageData;
	if(!FFileHelper::LoadFileToArray(SourceImageData, *ImagePath))
		return false;

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapper(SourceImageData);
	if(!ImageWrapper.IsValid())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to Get image wrapper"))
		return false;
	}
	
	OutPixels.Empty();
	OutSizeX = ImageWrapper->GetWidth();
	OutSizeY = ImageWrapper->GetHeight();
	
	
	TArray<uint8> UncompressedImageData;
	if(ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedImageData))
	{
		for(int32 i=0; i<UncompressedImageData.Num(); i+=4)
		{
			OutPixels.Add(FColor(
				UncompressedImageData[i+0],
				UncompressedImageData[i+1],
				UncompressedImageData[i+2],
				UncompressedImageData[i+3]
			));
		}
	}

	if(OutPixels.IsEmpty())
		return false;
	
	return true;
}

bool UBFL_Utility::GetImageRawData(TArray<uint8>& UncompressedImageData, ERGBFormat DestImageFormat, const FString& ImagePath)
{
	UncompressedImageData.Empty();
	
	if(!FPaths::FileExists(ImagePath))
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Invalid image path"));
		return false;
	}

	TArray<uint8> SourceImageData;
	if(!FFileHelper::LoadFileToArray(SourceImageData, *ImagePath))
		return false;

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapper(SourceImageData);
	if(!ImageWrapper.IsValid())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to Get image wrapper"))
		return false;
	}

	if(!ImageWrapper->GetRaw(DestImageFormat, 8, UncompressedImageData))
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to get raw image data"))
		return false;
	}
	if(UncompressedImageData.IsEmpty())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to get raw image data"))
		return false;
	}
	return true;
}







void UBFL_Utility::SaveTextureByImage(UTexture2D*& Texture, const FString& ImagePath)
{
	TArray<FColor> Pixels;
	int32 SizeX, SizeY;
	if(GetImagePixels(Pixels,SizeX,SizeY, ImagePath))
	{
		FString NewTexName = FPaths::GetBaseFilename(ImagePath);
		FString NewTexPath = FString("/Game/");
		FString NewTexPackageName = NewTexPath + NewTexName;

		UPackage* NewTexturePackage = CreatePackage(*NewTexPackageName);
		if(!NewTexturePackage)
		{
			UE_LOG(BFLUtilityLog, Error, TEXT("Failed to create package"));
			return;
		}
		FCreateTexture2DParameters Params;
		Params.bUseAlpha = true;
		Params.CompressionSettings = TC_Default;
		Params.TextureGroup = TEXTUREGROUP_World;
		Params.MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

		Texture = FImageUtils::CreateTexture2D(
			SizeX, SizeY,
			Pixels,
			NewTexturePackage,
			NewTexName,
			RF_Public | RF_Standalone,
			Params
		);
		if(Texture)
		{
			FAssetRegistryModule::AssetCreated(Texture);
			if(!NewTexturePackage->MarkPackageDirty())
			{
				UE_LOG(BFLUtilityLog, Error, TEXT("Failed to mark package dirty"));
			}
		}
	}
}

void UBFL_Utility::LoadImageToTexture2D(UTexture2D*& Texture, const FString& ImagePath)
{
	//Texture = FImageUtils::ImportFileAsTexture2D(ImagePath);//
	

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapper(ImagePath);
	if(!ImageWrapper.IsValid())
	{
		UE_LOG(BFLUtilityLog, Error, TEXT("Failed to Get image wrapper"))
		return;
	}
	int32 SizeX = ImageWrapper->GetWidth();
	int32 SizeY = ImageWrapper->GetHeight();
	Texture = UTexture2D::CreateTransient(SizeX, SizeY, PF_B8G8R8A8);

	if(Texture)
	{
		TArray<uint8> UncompressedImageData;
		if(!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedImageData))
		{
			UE_LOG(BFLUtilityLog, Error, TEXT("Failed to get raw image data"))
			return;
		}
		//填充数据
		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, UncompressedImageData.GetData(), UncompressedImageData.Num());
		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

		Texture->UpdateResource();		
	}
}


