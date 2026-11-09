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

void AProtoGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		ActivePlayerPawns.Add(Cast<AProtoCharacter>(NewPlayer->GetPawn()));
	}
}