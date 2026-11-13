// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Interface/DamageableInterface.h"
#include "LifeComponent.h"
#include "ProtoPlayerState.h"
#include "ProtoCharacter.generated.h"

UCLASS()
class COUCHGAME_API AProtoCharacter : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProtoCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Couch|Player", meta = (ToolTip = "Couch player index assigned during character initialization."))
	int32 CouchPlayerIndex = INDEX_NONE;
	
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Couch|Player", meta = (AllowPrivateAccess = "true"))
	mutable TObjectPtr<AProtoPlayerState> CachedCouchPlayerState;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Attempts to add coins to this character's Couch player state.
	virtual bool TryCollectCurrency_Implementation(const int32 Amount) override;
	// Attempts to remove coins from this character's Couch player state.
	virtual bool TryRemoveCurrency_Implementation(const int32 Amount) override;
	// Removes the requested coins, then applies the supplied life damage.
	virtual bool TryTakeDamage_Implementation(float Damage, float RagdollPushForce, FHitResult HitResult) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Couch|Player",
	meta = (ToolTip = "Life component that tracks this character's health and death state."))
	TObjectPtr<class ULifeComponent> LifeComponent;
	
	UFUNCTION(BlueprintCallable, Category = "Couch|Player")
	AProtoPlayerState* GetCouchPlayerState() const;
	
	UFUNCTION(BlueprintPure, Category = "Couch|Player")
	int32 GetCouchPlayerIndex() const;
	
private:
	bool IsDeadOrMissingLife() const;
};
