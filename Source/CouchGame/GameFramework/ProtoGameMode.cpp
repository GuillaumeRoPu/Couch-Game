// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoGameMode.h"

#include "ProtoGameHUD.h"
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
	// Player index 0 is normally created automatically.
	// Create player index 1 and immediately spawn its pawn.
	if (GetWorld())
	{
		UGameplayStatics::CreatePlayer(GetWorld(), 1, true);
	}
	//Disable the split screen for now, we will manage it later
	GetGameInstance()->GetGameViewportClient()->SetForceDisableSplitscreen(true);;
}

void AProtoGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
	
	if (AProtoCharacter* Character = Cast<AProtoCharacter>(NewPlayer->GetPawn()))
	{
		ActivePlayerPawns.AddUnique(Character);
		Character->LifeComponent->OnDied.AddUniqueDynamic(this, &AProtoGameMode::OnPlayerDied);
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