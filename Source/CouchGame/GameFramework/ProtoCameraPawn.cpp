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

void AProtoCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bAutoFrame)
	{
		UpdateFramingTarget(GameMode->ActivePlayerPawns);
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, FramingInterpSpeed));
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength,
									  DeltaTime, FramingInterpSpeed);
	}
}

bool AProtoCameraPawn::PrepareTargetLocations(
	const TArray<TObjectPtr<AProtoCharacter>>& InActors)
{
	FVector TargetLocationSum = FVector::ZeroVector;
	int32 ValidActorCount = 0;
	for (AProtoCharacter* Actor : InActors)
	{
		// UE: IsValid also rejects UObject references that are pending destruction, not just null pointers.
		if (IsValid(Actor))
		{
			TargetLocationSum += Actor->GetActorLocation();
			++ValidActorCount;
		}
	}

	if (ValidActorCount == 0)
	{
		return false;
	}

	TargetLocation = TargetLocationSum / static_cast<float>(ValidActorCount);
	return true;
}

void AProtoCameraPawn::UpdateFramingTarget(const TArray<TObjectPtr<AProtoCharacter>>& InActors)
{
	//Step by step: void APartyArenaCameraPawn::UpdateFramingTargets(const TArray<TObjectPtr<APartyCharacter>>& InActors)
	// STEP1: Setup the base variable useful for the whole function: CameraRightVector, CameraUpVector, init max horizontal/vertical spread to 0.
	if (!PrepareTargetLocations(InActors))
	{
		return;
	}
	
	const FVector CameraRight = Camera->GetRightVector();
	const FVector CameraUp = Camera->GetUpVector();
	float MaxHorizontalSpread = 0.0f;
	float MaxVerticalSpread = 0.0f;
	
	// STEP2: Find the furthest actor sideways and the furthest actor vertically from center.
	for (AProtoCharacter* Actor : InActors)
	{
	   if (IsValid(Actor))
	   {
	      const FVector Offset = Actor->GetActorLocation() - TargetLocation;
	
	      // Component of Offset along the camera's right axis, sign discarded (distance only).
	      MaxHorizontalSpread = FMath::Max(MaxHorizontalSpread, FMath::Abs(FVector::DotProduct(Offset, CameraRight)));
	
	      // Same idea, but along the camera's up axis.
	      MaxVerticalSpread = FMath::Max(MaxVerticalSpread, FMath::Abs(FVector::DotProduct(Offset, CameraUp)));
	   }
	}
	
	// STEP 3: Create the aspect ratio variable (get it from the cam OR 16/9)
	const float AspectRatio = Camera->bConstrainAspectRatio && Camera->AspectRatio > 0.0f ? Camera->AspectRatio : 16.f/9.f;
	
	// STEP 4: Tan of the half-angle: how much view width the camera covers per unit of distance.
	const float HorizontalFovTangent = FMath::Tan(FMath::DegreesToRadians(Camera->FieldOfView) * 0.5f);
	const float VerticalFovTangent = HorizontalFovTangent / AspectRatio;
	
	//STEP 5: calculate the RequiredHorizontalArmLength: It's the MaxHorizontalSpread (from the actor loop)+The WorldScreenMargin.X divided by the HorizontalFovTangent.
	const float RequiredHorizontalArmLength = (MaxHorizontalSpread + WorldScreenMargin.X) / FMath::Max(HorizontalFovTangent, UE_SMALL_NUMBER);
	
	//STEP 6: calculate the RequiredVerticalArmLength: It's the MaxVerticalSpread (from the actor loop)+The WorldScreenMargin.Y divided by the VerticalFovTangent.
	const float RequiredVerticalArmLength = (MaxVerticalSpread + WorldScreenMargin.Y) / FMath::Max(VerticalFovTangent, UE_SMALL_NUMBER);
	
	// Step 7: Set the final RequiredArmLength to be the greatest between the RequiredHorizontalArmLength and RequiredVerticalArmLength.
	// The tighter of the two constraints wins, satisfying it satisfies the other automatically.
	const float RequiredArmLength = FMath::Max(RequiredHorizontalArmLength, RequiredVerticalArmLength);
	
	//Step 8: Secure your TargetArmLenght variable to it respect the MinimumTargetArmLength,MaxArmLength or unlimited arm lenght.
	const float MinimumTargetArmLength = FMath::Max(MinArmLength, RequiredArmLength);
	if (MaxArmLength >= 0.0f)
	{
		TargetArmLength = FMath::Min(MinimumTargetArmLength, FMath::Max(MinArmLength, MaxArmLength));
	}
	else
	{
		TargetArmLength = MinimumTargetArmLength;
	}
}

