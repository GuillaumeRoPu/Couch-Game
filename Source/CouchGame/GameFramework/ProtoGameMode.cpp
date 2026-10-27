// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoGameMode.h"

#include "ProtoGameHUD.h"
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
