// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COUCHGAME_API IDamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Couch|Damage")
	bool TryTakeDamage(float Damage, float RagdollPushForce, FHitResult HitResult);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Couch|Currency")
	bool TryCollectCurrency(int32 Amount);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Couch|Currency")
	bool TryRemoveCurrency(int32 Amount);
};
