// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProtoGameMode.generated.h"

class AProtoCharacter;

/**
 * 
 */
UCLASS()
class COUCHGAME_API AProtoGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AProtoGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	//Player pawns that are currently active in the world, updates everytime a new player logs in
	UPROPERTY(Transient)
	TArray<TObjectPtr<AProtoCharacter>> ActivePlayerPawns;
	
protected:
	virtual void BeginPlay() override;
};
