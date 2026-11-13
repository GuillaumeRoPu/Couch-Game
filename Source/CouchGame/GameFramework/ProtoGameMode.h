// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Players/LifeComponent.h"
#include "ProtoGameMode.generated.h"

class AProtoCharacter;

UENUM(BlueprintType)
enum class RespawnType : uint8
{
	OnBaseRespawn,
	OnPlayer,
	OnCheckPoint
};

/**
 * 
 */
UCLASS()
class COUCHGAME_API AProtoGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AProtoGameMode();
	
	//Player pawns that are currently active in the world, updates everytime a new player logs in
	UPROPERTY(Transient)
	TArray<TObjectPtr<AProtoCharacter>> ActivePlayerPawns;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|Players")
	bool bRespawnOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|Players")
	RespawnType RespawnAtRandomPlayerStart = RespawnType::OnBaseRespawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|Players", meta = (ClampMin = "0.0", EditCondition = "bRespawnOnDeath"))
	float RespawnDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|Players",
	   meta = (ClampMin = "0.0", EditCondition = "bRespawnOnDeath"))
	float DeadPawnLifetime = 5.0f;
	
	UFUNCTION()
	void OnPlayerDied(ACharacter* Character, AController* DamageInstigator, AActor* DamageCauser, FCouchDamageContext Context);
	
protected:
	virtual void BeginPlay() override;
	
	/* Called whenever the player finishes spawning, we use it  */
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	
	/* Destroys the dead pawn and restarts the controller after the respawn delay. */
	void CreateNewPlayer(AController* PlayerController);
};
