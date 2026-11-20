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
	COLLISION = 0 UMETA(DisplayName = "COLLISION"),
	CONE_TRACE = 1 UMETA(DisplayName = "CONE_TRACE"),
	SPHERE_TRACE = 2  UMETA(DisplayName = "SPHERE_TRACE"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class COUCHGAME_API UMagnetComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMagnetComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Collision")
	E_MagnetCollisionType CollisionType = E_MagnetCollisionType::COLLISION;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Collision", meta=(EditConditionHides, EditCondition="CollisionType == E_MagnetCollisionType::CONE_TRACE"))
	float Height = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Collision", meta=(EditConditionHides, EditCondition="CollisionType == E_MagnetCollisionType::CONE_TRACE"))
	float DistanceFront = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Collision", meta=(EditConditionHides, EditCondition="CollisionType == E_MagnetCollisionType::CONE_TRACE"))
	float DistanceFrontSides = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Collision", meta=(EditConditionHides, EditCondition="CollisionType == E_MagnetCollisionType::CONE_TRACE"))
	float DistanceBackSides = 100.0f;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Collision", meta=(EditConditionHides, EditCondition="CollisionType == E_MagnetCollisionType::SPHERE_TRACE"))
	float Radius = 100.0f;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Power")
	E_MagnetPowerType PowerType = E_MagnetPowerType::REPULSE;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Power", meta=(EditConditionHides, EditCondition="CollisionType == E_MagnetCollisionType::COLLISION"))
	TArray<TSubclassOf<AActor>> TargetClasses;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Power")
	float Power = 20.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Power")
	bool isPowerConstant = false;
	
	UPROPERTY(BlueprintReadWrite)
	FVector ActionDirection = FVector(0.0f, 0.0f, 0.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	bool HasObject = false;
	TArray<AActor*> Objects;
	AActor* OtherActor = nullptr;
	
};
