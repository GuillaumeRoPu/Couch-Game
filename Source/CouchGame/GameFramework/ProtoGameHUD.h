// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Backend/Subsystem/CouchGameFlowSubsystem.h"
#include "GameFramework/HUD.h"
#include "ProtoGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class COUCHGAME_API AProtoGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	bool bShowMinigame = true;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION(BlueprintCallable, Category = "Couch|GameFlow")
	void RequestTransitionToState(ECouchGameFlowState NewState);
	
	UPROPERTY(Transient)
	TObjectPtr<UCouchGameFlowSubsystem> CouchGameFlowSubsystem;
};
