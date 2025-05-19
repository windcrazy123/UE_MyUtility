// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BFL_Utility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(BFLUtilityLog, Log, Log);

enum class ERGBFormat : int8;
enum class EImageFormat : int8;
/**
 * 
 */
UCLASS()
class TES_AOTEMAN_API UBFL_Utility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility | Timer")
	static float GetCurrentTime();

	
#if WITH_EDITOR
	
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Image")
	static void ImportImage(const TArray<FString>& SourcePaths, const FString& DestinationPath);
	
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Texture")
	static void ImportTexture(const TArray<FString>& SourcePaths, const FString& DestinationPath);
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Texture")
	static void ExportTexture(const TArray<FString>& SourcePaths, const FString& DestinationPath);

	//没写完 lod bias感觉做不出来 MipData一直获取不到
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Texture", meta=(DisplayName="Resize Texture With LODBias"))
	static void ResizeSelectedTexture_Mip(int32 LODBias = 1);
	//可用，修改贴图Size并在当前目录的Resize文件夹新增此贴图
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Texture", meta=(DisplayName="Resize Texture With SizeXY"))
	static void ResizeSelectedTexture_Img(int32 TargetSizeX, int32 TargetSizeY);
	/**
	 * 可用，修改贴图Size并替换
	 * @param bUseMaxInGame 如果为True将使用贴图细节面板中的 Max In-Game 的大小，如果为False将使用参数中指定的大小
	 */
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Texture", meta=(DisplayName="Resize Texture And Replace"))
	static void ResizeSelectedTexture_Replace(int32 TargetSizeX, int32 TargetSizeY, bool bUseMaxInGame = false);

	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Texture")
	static bool GetTexturePixels(TArray<FColor>& OutPixels, UTexture2D* OriginalTexture, int32 DestSizeX = 0, int32 DestSizeY = 0);
	UFUNCTION(BlueprintCallable, Category = "EditorUtility | Asset | Image")
	static bool GetImagePixels(TArray<FColor>& OutPixels, int32& OutSizeX, int32& OutSizeY, const FString& ImagePath);

	
#endif
private:
#if WITH_EDITOR
	
	static bool ResizeTexture(UTexture2D* Texture, int32 LODBias);
	static UTexture2D* ResizeTexture(UTexture2D* Texture, int32 TargetSizeX, int32 TargetSizeY, bool bReplace);
	
#endif
	
	[[nodiscard]]static TSharedPtr<class IImageWrapper> GetImageWrapper(TArray<uint8>& SourceImageData);
	[[nodiscard]]static TSharedPtr<class IImageWrapper> GetImageWrapper(const FString& ImagePath);
	static bool GetImageRawData(TArray<uint8>& UncompressedImageData, ERGBFormat DestImageFormat, const FString& ImagePath);

	
	//TEST============TEST============TEST============TEST============TEST
	UFUNCTION(BlueprintCallable, Category = "UtilityTest | Asset | Image")
	static void SaveTextureByImage(UTexture2D*& Texture, const FString& ImagePath);
	UFUNCTION(BlueprintCallable, Category = "UtilityTest | Asset | Image")
	static void LoadImageToTexture2D(UTexture2D*& Texture, const FString& ImagePath);
	UFUNCTION(BlueprintCallable, Category = "UtilityTest | Asset | Image")
	static void ResizeTest(const TArray<UTexture*>& InTextures, int32 InMaxSize);
	UFUNCTION(BlueprintCallable, Category = "UtilityTest | Asset | Image")
	static void ReplaceRefTest(UTexture2D* Texture, UTexture2D* OtherTexture);
};

