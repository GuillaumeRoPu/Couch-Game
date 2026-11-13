// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MagnetComponent.generated.h"

UENUM(BlueprintType)
enum class E_MagnetPowerType : uint8 {
     ATTRACT = 0 UMETA(DisplayName = "ATTRACT"),
     REPULSE = 1  UMETA(DisplayName = "REPULSE"),
     IS_ATTRACTED = 2 UMETA(DisplayName = "IS_ATTRACTED"),
};

UENUM(BlueprintType)
enum class E_MagnetCollisionType : uint8 {
     ATTRACT = 0 UMETA(DisplayName = "CONE_TRACE"),
     REPULSE = 1  UMETA(DisplayName = "SPHERE_TRACE"),
     IS_ATTRACTED = 2 UMETA(DisplayName = "COLLISION"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class COUCHGAME_API UMagnetComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMagnetComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
