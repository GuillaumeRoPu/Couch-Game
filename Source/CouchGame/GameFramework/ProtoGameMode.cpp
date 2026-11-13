// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoGameMode.h"

#include "ProtoGameHUD.h"
#include "Backend/Subsystem/CouchLobbySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Players/ProtoPlayerState.h"
#include "Players/ProtoPlayerController.h"
#include "Players/ProtoCharacter.h"

AProtoGameMode::AProtoGameMode()
{
	DefaultPawnClass = AProtoCharacter::StaticClass();
	PlayerControllerClass = AProtoPlayerController::StaticClass();
	PlayerStateClass = AProtoPlayerState::StaticClass();
	HUDClass = AProtoGameHUD::StaticClass();
}

void AProtoGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UCouchLobbySubsystem* LobbySubsystem = GameInstance->GetSubsystem<UCouchLobbySubsystem>())
		{
			const int32 ExtraPlayersToCreate = LobbySubsystem->GetJoinedPlayerCount() - 1;
			for (int32 PlayerIndex = 0; PlayerIndex < ExtraPlayersToCreate; ++PlayerIndex)
			{
				// -1 lets the engine assign the next available controller id. Controller id and
				// Couch player index are independent, the lobby ties a local player to a Couch
				// slot, not this id, so it doesn't matter which id lands where.
				UGameplayStatics::CreatePlayer(GetWorld(), -1, true);
			}
		}
	}

	//Disable the split screen
	GetGameInstance()->GetGameViewportClient()->SetForceDisableSplitscreen(true);;
}

void AProtoGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);

	if (APlayerController* PlayerController = Cast<APlayerController>(NewPlayer))
	{
		if (AProtoCharacter* Character = Cast<AProtoCharacter>(PlayerController->GetPawn()))
		{
			ActivePlayerPawns.AddUnique(Character);
			Character->LifeComponent->OnDied.AddUniqueDynamic(this, &AProtoGameMode::OnPlayerDied);
			
			AProtoPlayerState* ProtoPlayerState = Character->GetCouchPlayerState();
			
			if (!ProtoPlayerState)
			{
				return;
			}

			UGameInstance* GameInstance = GetGameInstance();
			UCouchLobbySubsystem* LobbySubsystem = GameInstance
				? GameInstance->GetSubsystem<UCouchLobbySubsystem>()
				: nullptr;
			if (!LobbySubsystem)
			{
				return;
			}

			int32 CouchPlayerIndex = INDEX_NONE;
			if (LobbySubsystem->ClaimNextUnassignedLocalPlayer(PlayerController->GetLocalPlayer(), CouchPlayerIndex))
			{
				ProtoPlayerState->SetCouchPlayerIndex(CouchPlayerIndex);
			}
		}
	}
}

void AProtoGameMode::CreateNewPlayer(AController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (APawn* DeadPawn = PlayerController->GetPawn())
	{
		PlayerController->UnPossess();
	}
	
	RestartPlayer(PlayerController);
}

void AProtoGameMode::OnPlayerDied(ACharacter* Character, AController* DamageInstigator, AActor* DamageCauser, FCouchDamageContext Context)
{
	if (AProtoCharacter* CouchCharacter = Cast<AProtoCharacter>(Character))
	{
		CouchCharacter->LifeComponent->OnDied.RemoveDynamic(this, &AProtoGameMode::OnPlayerDied);
		
		ActivePlayerPawns.Remove(CouchCharacter);

		if (bRespawnOnDeath)
		{
			if (AController* PlayerController = CouchCharacter->GetController())
			{
				if (RespawnDelay <= 0.0f)
				{
					CreateNewPlayer(PlayerController);
				}
				else
				{
					FTimerHandle RespawnTimerHandle;
					FTimerDelegate RespawnDelegate;
					RespawnDelegate.BindUObject(this,&AProtoGameMode::CreateNewPlayer,PlayerController);
					GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate,RespawnDelay,false);
				}
			}
			if (IsValid(CouchCharacter))
			{
				const float MinimumRagdollLifetime = bRespawnOnDeath ? RespawnDelay : 0.0f;
				const float RagdollLifetime = FMath::Max(DeadPawnLifetime, MinimumRagdollLifetime);
				CouchCharacter->LifeComponent->RetireForRespawn(RagdollLifetime);
				CouchCharacter = nullptr;
			}
		}
	}
}