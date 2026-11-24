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
	RespawnType RespawnAtRandomPlayerStart = RespawnType::OnPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|Players", meta = (ClampMin = "0.0", EditCondition = "bRespawnOnDeath"))
	float RespawnDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|Players",
	   meta = (ClampMin = "0.0", EditCondition = "bRespawnOnDeath"))
	float DeadPawnLifetime = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|EditorOnly")
	bool bConnectOnEditor = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Couch|EditorOnly")
	bool StartWithMainMenu = true;
	
	UFUNCTION()
	void OnPlayerDied(ACharacter* Character, AController* DamageInstigator, AActor* DamageCauser, FCouchDamageContext Context);
	
	UFUNCTION(BlueprintCallable, Category = "Couch|HUD")
	bool IsHUDAlreadySpawned() { return bHUDAlreadySpawned; }
	
	UFUNCTION(BlueprintCallable, Category = "Couch|HUD")
	void HUDSpawned();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	//Called whenever the player has finished its restart
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	
	//Dead pawn gets disconnected with player controller
	void CreateNewPlayer(AController* PlayerController);
	
	bool bHUDAlreadySpawned = false;
};
