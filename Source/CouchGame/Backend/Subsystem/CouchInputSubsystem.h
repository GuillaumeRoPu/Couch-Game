// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CouchInputSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyboardInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamepadInput);

/**
 * 
 */
UCLASS()
class COUCHGAME_API UCouchInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnKeyboardInput OnKeyboardInput;
	UPROPERTY(BlueprintAssignable)
	FOnGamepadInput OnGamepadInput;
};
