// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/ProtoCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "ProtoGameMode.h"
#include "Players/ProtoCharacter.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AProtoCameraPawn::AProtoCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SetRootComponent(SpringArm);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-45, 45, 0.0f));
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = FramingInterpSpeed;
	SpringArm->TargetArmLength = TargetArmLength;

	Camera->FieldOfView = 65.0f;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
}

void AProtoCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<AProtoGameMode>(GetWorld()->GetAuthGameMode());
}

void AProtoCameraPawn::SetCameraRotation(const FRotator* Rotator) const
{
	if (Camera)	
	{
		Camera->SetRelativeRotation(*Rotator);
	}
}

void AProtoCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bAutoFrame)
	{
		UpdateFramingTargets(GameMode->ActivePlayerPawns);
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, FramingInterpSpeed));
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, FramingInterpSpeed);
		//Computing the new target of the camera based on how far away the characters are with each other
		TArray<FVector> OutTargetLocations;
		float Dist = 0;
		for (int32 i = 0; i < 2; i++)
		{
			
		}
		FVector NewActorLoc = GetActorLocation() + (0,1 * Dist) * Camera->GetForwardVector();
		SetActorLocation(NewActorLoc);
	}
}

bool AProtoCameraPawn::PrepareTargetLocations(const TArray<TObjectPtr<AProtoCharacter>>& InActors, TArray<FVector>& OutTargetLocations)
{
	// Keep only live actors and use their average location as the initial framing center.
	OutTargetLocations.Reset();
	OutTargetLocations.Reserve(InActors.Num());
	FVector TargetLocationSum = FVector::ZeroVector;
	for (AProtoCharacter* Actor : InActors)
	{
		// UE: IsValid also rejects UObject references that are pending destruction, not just null pointers.
		if (IsValid(Actor))
		{
			const FVector ActorLocation = Actor->GetActorLocation();
			OutTargetLocations.Add(ActorLocation);
			TargetLocationSum += ActorLocation;
		}
	}

	if (OutTargetLocations.IsEmpty())
	{
		return false;
	}

	TargetLocation = TargetLocationSum / static_cast<float>(OutTargetLocations.Num());
	// Y'avait une idée mais vsy c trop dur
	//float NewDist = FVector::Dist(Camera->GetComponentLocation(), TargetLocation);
	//int32 It = 0;
	//for (FVector Loc : OutTargetLocations)
	//{
	//	float LocDist = FVector::Dist(Camera->GetComponentLocation(), Loc);
	//	OutTargetLocations[It] = OutTargetLocations[It] * (LocDist / NewDist);
	//	It++;
	//}
	return true;
}

void AProtoCameraPawn::UpdateFramingTargets(const TArray<TObjectPtr<AProtoCharacter>>& InActors)
{
	// Build the desired camera center and arm length from the current actor group.
	TArray<FVector> TargetLocations;
	if (!PrepareTargetLocations(InActors, TargetLocations))
	{
		return;
	}

	const FVector CurrentCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	const FVector ArmDirection = CurrentCameraOffset.IsNearlyZero()
	                             ? -Camera->GetForwardVector()
	                             : CurrentCameraOffset.GetSafeNormal();
	// const FVector ArmDirection = CurrentCameraOffset.IsNearlyZero() ? TargetLocation - CurrentCameraOffset : CurrentCameraOffset.GetSafeNormal();
	const FVector CameraForward = Camera->GetForwardVector();
	const FVector CameraRight = Camera->GetRightVector();
	const FVector CameraUp = Camera->GetUpVector();
	float HorizontalLimit = 0.0f;
	float VerticalLimit = 0.0f;
	constexpr float ProjectionHalfAngleFactor = 0.5f;
	CalculateProjectionLimits(CameraForward, HorizontalLimit, VerticalLimit);

	float HorizontalShift = 0.0f;
	float VerticalShift = 0.0f;
	float MaxLeftDepthRequirement = 0.0f;
	float MaxRightDepthRequirement = 0.0f;
	float MaxBottomDepthRequirement = 0.0f;
	float MaxTopDepthRequirement = 0.0f;
	ApplyScreenSpaceFraming(TargetLocations, CameraForward, CameraRight, CameraUp,
	                        HorizontalLimit, VerticalLimit, ProjectionHalfAngleFactor,
	                        HorizontalShift, VerticalShift,
	                        MaxLeftDepthRequirement, MaxRightDepthRequirement,
	                        MaxBottomDepthRequirement, MaxTopDepthRequirement);
	UpdateArmLength(ArmDirection, CameraForward, HorizontalLimit, VerticalLimit,
	                HorizontalShift, VerticalShift, MaxLeftDepthRequirement, MaxRightDepthRequirement,
	                MaxBottomDepthRequirement, MaxTopDepthRequirement);
}

void AProtoCameraPawn::CalculateProjectionLimits(const FVector& CameraForward,
	float& OutHorizontalLimit, float& OutVerticalLimit) const
{
	// Derive the screen-space limits from the active viewport, camera projection, and world margin.
	// Calculate these before recentering; the recentering only changes the camera's right/up axes.

	float AspectRatio = 16.0f / 9.0f;
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		const FIntPoint ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
		if (ViewportSize.X > 0 && ViewportSize.Y > 0)
		{
			AspectRatio = static_cast<float>(ViewportSize.X) / static_cast<float>(ViewportSize.Y);
		}
	}

	if (Camera->bConstrainAspectRatio && Camera->AspectRatio > 0.0f)
	{
		AspectRatio = Camera->AspectRatio;
	}

	// UE: camera FOV is horizontal, so derive vertical FOV from the current aspect ratio.
	constexpr float HorizontalFovMinDegrees = 1.0f;
	constexpr float HorizontalFovMaxDegrees = 179.0f;
	constexpr float ProjectionHalfAngleFactor = 0.5f;
	constexpr float ProjectionDiameterFactor = 2.0f;
	constexpr float MinimumDepth = 0.01f;
	constexpr float MinimumProjectionFactor = 0.01f;
	const float HorizontalFovRadians = FMath::DegreesToRadians(
		FMath::Clamp(Camera->FieldOfView, HorizontalFovMinDegrees, HorizontalFovMaxDegrees));
	const float HorizontalFovTangent = FMath::Tan(HorizontalFovRadians * ProjectionHalfAngleFactor);
	const float VerticalFovRadians = ProjectionDiameterFactor
		* FMath::Atan(HorizontalFovTangent / AspectRatio);
	const float VerticalFovTangent = FMath::Tan(VerticalFovRadians * ProjectionHalfAngleFactor);
	// Convert the world-space margin into a screen-space margin at the current depth.
	const float CurrentCameraDepth = FMath::Max(
		MinimumDepth, FVector::DotProduct(TargetLocation - Camera->GetComponentLocation(), CameraForward));
	const float HorizontalHalfExtent = CurrentCameraDepth * HorizontalFovTangent;
	const float VerticalHalfExtent = CurrentCameraDepth * VerticalFovTangent;
	const float HorizontalMargin = FMath::Clamp(
		WorldScreenMargin.X / (ProjectionDiameterFactor * HorizontalHalfExtent), 0.0f, 0.49f);
	const float VerticalMargin = FMath::Clamp(
		WorldScreenMargin.Y / (ProjectionDiameterFactor * VerticalHalfExtent), 0.0f, 0.49f);

	// These limits are the maximum screen offset/depth slopes allowed by the projection.
	OutHorizontalLimit = HorizontalFovTangent * FMath::Max(
		MinimumProjectionFactor, 1.0f - ProjectionDiameterFactor * HorizontalMargin);
	OutVerticalLimit = VerticalFovTangent * FMath::Max(
		MinimumProjectionFactor, 1.0f - ProjectionDiameterFactor * VerticalMargin);
}


void AProtoCameraPawn::ApplyScreenSpaceFraming(const TArray<FVector>& TargetLocations,
	const FVector& CameraForward, const FVector& CameraRight, const FVector& CameraUp,
	const float HorizontalLimit, const float VerticalLimit,
	const float ProjectionHalfAngleFactor,
	float& OutHorizontalShift, float& OutVerticalShift,
	float& OutMaxLeftDepthRequirement, float& OutMaxRightDepthRequirement,
	float& OutMaxBottomDepthRequirement, float& OutMaxTopDepthRequirement)
{
	// Find the screen-space center that balances the nearest edge requirements for every actor.
	// Recenter the group in the camera's 2D plane while accounting for actor depth.
	float MaxLeftDepthRequirement = TNumericLimits<float>::Lowest();
	float MaxRightDepthRequirement = TNumericLimits<float>::Lowest();
	float MaxBottomDepthRequirement = TNumericLimits<float>::Lowest();
	float MaxTopDepthRequirement = TNumericLimits<float>::Lowest();
	for (const FVector& ActorLocation : TargetLocations)
	{
		const FVector PointFromTarget = ActorLocation - TargetLocation;
		const float PointForwardOffset = FVector::DotProduct(PointFromTarget, CameraForward);
		const float ScreenX = FVector::DotProduct(PointFromTarget, CameraRight);
		const float ScreenY = FVector::DotProduct(PointFromTarget, CameraUp);

		// Include depth so the framing remains stable as the camera moves.
		MaxLeftDepthRequirement = FMath::Max(
			MaxLeftDepthRequirement,
			-ScreenX / HorizontalLimit - PointForwardOffset);
		MaxRightDepthRequirement = FMath::Max(
			MaxRightDepthRequirement,
			ScreenX / HorizontalLimit - PointForwardOffset);
		MaxBottomDepthRequirement = FMath::Max(
			MaxBottomDepthRequirement,
			-ScreenY / VerticalLimit - PointForwardOffset);
		MaxTopDepthRequirement = FMath::Max(
			MaxTopDepthRequirement,
			ScreenY / VerticalLimit - PointForwardOffset);
	}

	// Balance the opposing requirements to find the best screen-space center.
	OutHorizontalShift =
		(HorizontalLimit * ProjectionHalfAngleFactor) * (MaxRightDepthRequirement - MaxLeftDepthRequirement);
	OutVerticalShift =
		(VerticalLimit * ProjectionHalfAngleFactor) * (MaxTopDepthRequirement - MaxBottomDepthRequirement);

	TargetLocation += CameraRight * OutHorizontalShift;
	TargetLocation += CameraUp * OutVerticalShift;
	OutMaxLeftDepthRequirement = MaxLeftDepthRequirement;
	OutMaxRightDepthRequirement = MaxRightDepthRequirement;
	OutMaxBottomDepthRequirement = MaxBottomDepthRequirement;
	OutMaxTopDepthRequirement = MaxTopDepthRequirement;
}

void AProtoCameraPawn::UpdateArmLength(const FVector& ArmDirection, const FVector& CameraForward,
	const float HorizontalLimit, const float VerticalLimit, const float HorizontalShift,
	const float VerticalShift, const float MaxLeftDepthRequirement,
	const float MaxRightDepthRequirement, const float MaxBottomDepthRequirement,
	const float MaxTopDepthRequirement)
{
	// Convert the framing requirements into a bounded spring-arm target length.
	// The arm moves opposite the camera's forward vector, so this is depth gained per unit.
	const float ArmDepthPerUnit = -FVector::DotProduct(ArmDirection, CameraForward);
	if (ArmDepthPerUnit <= UE_SMALL_NUMBER)
	{
		TargetArmLength = MinArmLength;
		return;
	}

	const float RequiredHorizontalDepth = FMath::Max(
		MaxLeftDepthRequirement + HorizontalShift / HorizontalLimit,
		MaxRightDepthRequirement - HorizontalShift / HorizontalLimit);
	const float RequiredVerticalDepth = FMath::Max(
		MaxBottomDepthRequirement + VerticalShift / VerticalLimit,
		MaxTopDepthRequirement - VerticalShift / VerticalLimit);
	const float RequiredArmLength = FMath::Max(
		MinArmLength,
		FMath::Max(RequiredHorizontalDepth, RequiredVerticalDepth) / ArmDepthPerUnit);

	// A negative maximum means the camera has no upper framing limit.
	TargetArmLength = MaxArmLength < 0.0f
		                  ? RequiredArmLength
		                  : FMath::Min(RequiredArmLength, FMath::Max(MinArmLength, MaxArmLength + 400));
}

