// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LifeComponent.generated.h"


USTRUCT(BlueprintType)
struct COUCHGAME_API FPartyDamageContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) TObjectPtr<AController> Instigator = nullptr;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor> DamageCauser = nullptr;
	UPROPERTY(BlueprintReadOnly) FVector ImpactPoint = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) FVector ImpactNormal = FVector::ZeroVector;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPartyDamageTaken, float, Damage, float, RemainingLife, AController*, Instigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPartyLifeReset, ACharacter*, Character, float, NewLifePoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPartyDied, ACharacter*, Character, AController*, Instigator, AActor*, DamageCauser, FPartyDamageContext, Context);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COUCHGAME_API ULifeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULifeComponent();
	
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

	UPROPERTY(BlueprintAssignable, Category = "Life|Events") FPartyDamageTaken OnDamageTaken;
	UPROPERTY(BlueprintAssignable, Category = "Life|Events") FPartyDied OnDied;
	UPROPERTY(BlueprintAssignable, Category = "Life|Events") FPartyLifeReset OnLifeReset;

	
	UFUNCTION(BlueprintPure, Category = "Life") bool IsDead() const { return bIsDead; }
	/** Restores life points and the owning character's movement and collision state after death. */
	UFUNCTION(BlueprintCallable, Category = "Life") void ResetLife();
	void RetireForRespawn(float RetireDelay);
	void ApplyDamage(float Damage, AController* Instigator = nullptr, AActor* DamageCauser = nullptr, const float RagdollPushForce = 0.f, const FHitResult& HitResult = FHitResult());

private:
	float GetNormalizedMaxLifePoints() const;
	void PlayDeathAudio(const ACharacter* Character) const;
	void ApplyRagdoll(ACharacter* Character, const FPartyDamageContext&, const float RagdollPushForce) const;
	void ApplyDeath(AController* Instigator, AActor* DamageCauser, const float RagdollPushForce, const FPartyDamageContext& Context);
	static constexpr float MinimumLifePoints = 0.01f;
	bool bIsDead = false;
};
