// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Backend/CouchDevSettings.h" 
#include "CouchLobbySubsystem.h"
#include "CouchGameFlowSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogCouchGameFlow, Log, All);


/**
 * 
 */
UCLASS()
class COUCHGAME_API UCouchGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
};
