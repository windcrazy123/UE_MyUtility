// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TextureImportUI.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Config=MyEditor,ProjectUserConfig, DisplayName="Texture Import Settings UI")
class STANDALONTEST_API UTextureImportUI : public UObject
{
	GENERATED_BODY()
public:
	//https://blog.csdn.net/a359877454/article/details/116297759
	UPROPERTY(EditAnywhere, Category="Basic", meta=(Tooltip="ResizeTexture"), Config)
	bool bTextureResize = false;
	UPROPERTY(EditAnywhere, Category="Basic", meta=(Tooltip="DestTexture", EditCondition="bTextureResize", EditConditionHides), Config)
	FVector2D TextureSize = FVector2D(512,512);

	//https://forums.unrealengine.com/t/how-to-properly-insert-engine-default-enums-in-fstructs/436545
	UPROPERTY(EditAnywhere, Category="Basic", meta=(Tooltip="CompressionSettings"))
	TEnumAsByte<TextureCompressionSettings> CompressionSettings = TC_Default;

	UPROPERTY(EditAnywhere, Category="Basic", meta=(Tooltip="MipGenSettings"))
	TEnumAsByte<TextureMipGenSettings> MipGenSettings = TMGS_FromTextureGroup;

	UPROPERTY(EditAnywhere, Category="Basic", meta=(Tooltip="TextureGroup"))
	TEnumAsByte<TextureGroup> TextureGroup = TEXTUREGROUP_World;

	// UPROPERTY(EditAnywhere, Category="Basic", meta=(Tooltip="Replace Existing File?"))
	// bool bReplaceExisting = false;

	UPROPERTY(EditAnywhere, Config)
	bool bSaveAsDefault = true;

	UFUNCTION(BlueprintCallable, Category = Miscellaneous)
	void ResetToDefault();
};
