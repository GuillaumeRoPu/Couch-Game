// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/MagnetComponent.h"
#include "Engine/GameEngine.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMagnetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

FVector UMagnetComponent::SetForceDirection(FVector MovingObjLoc, FTransform center, float force)
{
	FVector forceDir = (MovingObjLoc - center.GetLocation()).GetSafeNormal();
	
	FVector centerUp = center.GetRotation().GetUpVector().GetSafeNormal();
	FVector centerRight = center.GetRotation().GetRightVector().GetSafeNormal();
	FVector centerForward = center.GetRotation().GetForwardVector().GetSafeNormal();
	
	double angleHigh = 0.0f;
	double angleRight = 0.0f;
	double angleForward = 0.0f;

	
	switch (ForceDirection)
	{
		case E_ForceDirection::DEFAULT_CONTROLLED:
			angleHigh = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(centerUp, forceDir)));
			angleRight = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(centerRight, forceDir)));
			angleForward = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(centerForward, forceDir)));
			
			

			//If object goes upward
			if (angleHigh <= 22.5)
			{
				forceDir = centerUp;
			}
			
			//If object goes diagonally upward
			else if (angleHigh <= 67.5)
			{
				//Force up forward
				if (angleForward <= 22.5)
				{
					forceDir = centerUp + centerForward;
				}
				//Force up backward
				else if (157.5 < angleForward)
				{
					forceDir = centerUp - centerForward;
				}
				//If object to the right
				else if (angleRight < 90)
				{
					//Force up right forward
					if (angleForward <= 67.5)
					{
						forceDir = centerUp + centerRight + centerForward;
					}
					//Force up right
					else if (angleForward <= 112.5)
					{
						forceDir = centerUp + centerRight;
					}
					//Force up right backward
					else if (angleForward < 157.5)
					{
						forceDir = centerUp + centerRight - centerForward;
					}
				}
				//If object to the left
				else if (angleRight > 90)
				{
					//Force up left forward
					if (angleForward <= 67.5)
					{
						forceDir = centerUp - centerRight + centerForward;
					}
					//Force up left
					else if (angleForward <= 112.5)
					{
						forceDir = centerUp - centerRight;
					}
					//Force up left backward
					else if (angleForward < 157.5)
					{
						forceDir = centerUp - centerRight - centerForward;
					}
				}
			}
		
			//If object goes straight
			else if (angleHigh <= 112.5)
			{
				//Force forward
				if (angleForward <= 22.5)
				{
					forceDir = centerForward;
				}
				//Force backward
				else if (157.5 < angleForward)
				{
					forceDir = -centerForward;
				}
				//If object to the right
				else if (angleRight < 90)
				{
					//Force right forward
					if (angleForward <= 67.5)
					{
						forceDir = centerForward + centerRight;
					}
					//Force right
					else if (angleForward <= 112.5)
					{
						forceDir = centerRight;
					}
					//Force right backward
					else if (angleForward < 157.5)
					{
						forceDir = -centerForward + centerRight;
					}
				}
				//If object to the left
				else if (angleRight > 90)
				{
					//Force left forward
					if (angleForward <= 67.5)
					{
						forceDir = centerForward - centerRight;
					}
					//Force left
					else if (angleForward <= 112.5)
					{
						forceDir = -centerRight;
					}
					//Force left backward
					else if (angleForward < 157.5)
					{
						forceDir = -centerForward - centerRight;
					}
				}
			}
			
		
			break;
			
		case E_ForceDirection::DEFAULT_UNCONTROLLED:
			break;
			
		case E_ForceDirection::FRONTWARD:
			forceDir = centerForward;
			break;

			
		case E_ForceDirection::RIGHTWARD:
			forceDir = centerRight;
			break;
			
			
		case E_ForceDirection::UPWARD:
			forceDir = centerUp;
			break;
	}
	switch (PowerType)
	{
		case E_MagnetPowerType::ATTRACT:
			forceDir = -(force * forceDir);
			break;
		case E_MagnetPowerType::REPULSE:
			forceDir = force * forceDir;
			break;
	}
	
	
	return forceDir;
}


// Called every frame
void UMagnetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

