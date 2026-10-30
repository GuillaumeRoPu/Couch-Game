// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Interface/DamageableInterface.h"
#include "LifeComponent.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual bool TryTakeDamage_Implementation(float Damage, FHitResult HitResult) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Couch|Player",
	meta = (ToolTip = "Life component that tracks this character's health and death state."))
	TObjectPtr<class ULifeComponent> LifeComponent;
};
