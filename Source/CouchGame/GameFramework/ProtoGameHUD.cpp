// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoGameHUD.h"

void AProtoGameHUD::BeginPlay()
{
	CouchGameFlowSubsystem = GetGameInstance()->GetSubsystem<UCouchGameFlowSubsystem>();
	
	Super::BeginPlay();
}

void AProtoGameHUD::RequestTransitionToState(ECouchGameFlowState NewState)
{
	CouchGameFlowSubsystem->RequestTransition(NewState);
}
