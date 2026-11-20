// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoGameHUD.h"

#include "ProtoGameMode.h"

void AProtoGameHUD::BeginPlay()
{
	CouchGameFlowSubsystem = GetGameInstance()->GetSubsystem<UCouchGameFlowSubsystem>();
	
	
	if (AProtoGameMode* GM = Cast<AProtoGameMode>(GetWorld()->GetAuthGameMode()))
	{
		bShowMinigame = GM->StartWithMainMenu;
	}

	
	Super::BeginPlay();
}

void AProtoGameHUD::RequestTransitionToState(ECouchGameFlowState NewState)
{
	CouchGameFlowSubsystem->RequestTransition(NewState);
}
