// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LifeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COUCHGAME_API ULifeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULifeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life", meta = (ClampMin = "0.01"))
	float MaxLifePoints = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Life")
	float CurrentLifePoints = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life", meta = (ClampMin = "0.0"))
	float DeathImpulseStrength = 30000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life|Effects")
	TObjectPtr<USoundBase> DeathSFX;

	void ApplyDamage(float Damage, AController* Instigator = nullptr, AActor* DamageCauser = nullptr, const FHitResult& HitResult = FHitResult());


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
