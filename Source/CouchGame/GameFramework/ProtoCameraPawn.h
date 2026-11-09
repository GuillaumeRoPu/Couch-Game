// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProtoCameraPawn.generated.h"

class AProtoCharacter;
class UCameraComponent;
class USpringArmComponent;
class AProtoGameMode;

UCLASS()
class COUCHGAME_API AProtoCameraPawn : public APawn
{
	GENERATED_BODY()
	
public:
	AProtoCameraPawn();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Camera",
		meta = (ToolTip = "Automatically adjusts the camera framing to keep the party in view."))
	bool bAutoFrame = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Camera",
		meta = (ClampMin = "0.0", EditCondition = "bAutoFrame", EditConditionHides,
			ToolTip = "Minimum spring-arm length used by automatic framing."))
	float MinArmLength = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Camera",
		meta = (ClampMin = "-1.0", EditCondition = "bAutoFrame", EditConditionHides,
			ToolTip = "Maximum spring-arm length; use -1 for no maximum."))
	float MaxArmLength = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Camera",
		meta = (ClampMin = "0.0", EditCondition = "bAutoFrame", EditConditionHides,
			ToolTip = "Horizontal and vertical world-space margins kept around the framed party."))
	FVector2D WorldScreenMargin = FVector2D(150.0f, 200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Camera",
		meta = (ClampMin = "0.0", EditCondition = "bAutoFrame", EditConditionHides,
			ToolTip = "Speed at which automatic framing moves toward its target."))
	float FramingInterpSpeed = 5.0f;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void BeginPlay() override;
	
	void SetCameraRotation(const FRotator* rotator) const;
	
	void UpdateFramingTargets(const TArray<TObjectPtr<AProtoCharacter>>& InActors);

private:
	// Filters null or pending-destruction actors, then calculates the average framing center from the rest.
	bool PrepareTargetLocations(const TArray<TObjectPtr<AProtoCharacter>>& InActors,
	                            TArray<FVector>& OutTargetLocations);
	// Converts the active viewport and camera projection into horizontal and vertical framing limits.
	void CalculateProjectionLimits(const FVector& CameraForward,
	                              float& OutHorizontalLimit,
	                              float& OutVerticalLimit) const;
	// Measures screen-space offsets and depth requirements needed to keep all targets within those limits.
	void ApplyScreenSpaceFraming(const TArray<FVector>& TargetLocations,
	                            const FVector& CameraForward,
	                            const FVector& CameraRight,
	                            const FVector& CameraUp,
	                            float HorizontalLimit,
	                            float VerticalLimit,
	                            float ProjectionHalfAngleFactor,
	                            float& OutHorizontalShift,
	                            float& OutVerticalShift,
	                            float& OutMaxLeftDepthRequirement,
	                            float& OutMaxRightDepthRequirement,
	                            float& OutMaxBottomDepthRequirement,
	                            float& OutMaxTopDepthRequirement);
	// Applies the calculated center shift and arm-length requirement, subject to the designer's limits.
	void UpdateArmLength(const FVector& ArmDirection,
	                    const FVector& CameraForward,
	                    float HorizontalLimit,
	                    float VerticalLimit,
	                    float HorizontalShift,
	                    float VerticalShift,
	                    float MaxLeftDepthRequirement,
	                    float MaxRightDepthRequirement,
	                    float MaxBottomDepthRequirement,
	                    float MaxTopDepthRequirement);
	//The target we will use to move the camera around
	FVector TargetLocation = FVector::ZeroVector;
	float TargetArmLength = 1400.0f;

	UPROPERTY(Transient)
	class AProtoGameMode* GameMode;
};
