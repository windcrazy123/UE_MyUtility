#pragma once

DECLARE_LOG_CATEGORY_EXTERN(StandalonTest, Log, Log);

class UTextureImportUI;
struct FCreateTexture2DParameters;

class StandalonTestUtils
{
public:
	static void ExecuteImportImage(const FString& CachedSavePath);
	static FString CachedSelectPath;
	//static bool bImportAll;
	static bool IsPathValid(const FString& CachedSavePath);
	//static void DoActualImport(const TArray<FString>& Files, const FString& TargetPath, TSharedPtr<UTextureImportUI> Settings);
	static void DoActualImport(const FString& File, const FString& TargetPath, UTextureImportUI* Settings);
	static void ApplyImportSettingToFactory(FCreateTexture2DParameters& Params, const UTextureImportUI* Settings);
	[[nodiscard]]static TSharedPtr<class IImageWrapper> GetImageWrapper(TArray<uint8>& SourceImageData);
	static void ReplaceReferences();
};
