// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "Tests/MagnetComponent.h"
#include "CouchPlayerAvatarData.generated.h"

/**
 * 
 */
UCLASS()
class COUCHGAME_API UCouchPlayerAvatarData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (ToolTip = "Stable identifier used to select this avatar."))
	FName AvatarId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (ToolTip = "Display name shown for this avatar"))
	FText FriendlyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Presentation", meta = (AssetBundles = "UI", ToolTip = "Icon displayed for this avatar"))
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Presentation", meta = (AssetBundles = "Game", ToolTip = "Skeletal mesh used to represent this avatar"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Presentation", meta = (AssetBundles = "Game", ToolTip = "Animation Blueprint to make the skeletal mesh move"))
	TSoftObjectPtr<UAnimBlueprint> AnimationBlueprintLibrary;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ToolTip = "Magnet powers use by this player avatar"))
	E_MagnetPowerType MagnetPowerType;

	// Loads the soft reference of the Icon Texture2D for this player
	UFUNCTION(BlueprintCallable, Category = "Avatar|Presentation")
	UTexture2D* LoadIconTexture() const;

	// Loads the soft reference of the SkeletalMesh for this player
	UFUNCTION(BlueprintCallable, Category = "Avatar|Presentation")
	USkeletalMesh* LoadSkeletalMesh() const;
	
	// Loads the soft reference of the Animation BP for this player
	UFUNCTION(BlueprintCallable, Category = "Avatar|Presentation")
	UAnimBlueprint* LoadAnimationBlueprint() const;
};
