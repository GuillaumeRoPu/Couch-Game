// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoPlayerController.h"

#include "EngineUtils.h"
#include "GameFramework/ProtoCameraPawn.h"

void AProtoPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (TActorIterator<AProtoCameraPawn> It(GetWorld()); It)
	{
		SetViewTarget(*It);
	}
}