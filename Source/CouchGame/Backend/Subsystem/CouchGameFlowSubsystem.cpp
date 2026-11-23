// Fill out your copyright notice in the Description page of Project Settings.


#include "CouchGameFlowSubsystem.h"

#include "Kismet/GameplayStatics.h"

//Simple initialize at the beginning of a gameplay session, we set up every single pointers we want to keep and also set up base value for some variables
void UCouchGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UCouchLobbySubsystem>();
	Super::Initialize(Collection);

	LobbySubsystem = GetGameInstance()->GetSubsystem<UCouchLobbySubsystem>();
	DevSettings = GetDefault<UCouchDevSettings>();
	CurrentState = ECouchGameFlowState::None;
	
	//Request transition from None to MainMenu cuz first thing that appears is supposed to be the MainMenu
	RequestTransition(ECouchGameFlowState::MainMenu);
}

//Reset the pointers from initialize
void UCouchGameFlowSubsystem::Deinitialize()
{
	LobbySubsystem = nullptr;
	DevSettings = nullptr;
	Super::Deinitialize();
}

//Before every transition, this should be called (unless forced transition). Checks every transition, and if it is allowed, sends true. Sends false otherwise.
bool UCouchGameFlowSubsystem::IsTransitionAllowed(ECouchGameFlowState From, ECouchGameFlowState To) const
{
	if (From == ECouchGameFlowState::None && To == ECouchGameFlowState::MainMenu) return true;
	if (From == ECouchGameFlowState::MainMenu && To == ECouchGameFlowState::Lobby) return true;
	if (From == ECouchGameFlowState::Lobby && To == ECouchGameFlowState::Gameplay) return true;
	if (From == ECouchGameFlowState::Lobby && To == ECouchGameFlowState::MainMenu) return true;
	if (From == ECouchGameFlowState::Gameplay && To == ECouchGameFlowState::MainMenu) return true;
	if (From == ECouchGameFlowState::Gameplay && To == ECouchGameFlowState::FinalScreen) return true;
	if (From == ECouchGameFlowState::FinalScreen && To == ECouchGameFlowState::MainMenu) return true;
	return false;
}

//In case we want to open a map (shouldn't have to)
void UCouchGameFlowSubsystem::OpenConfiguredMap(const TSoftObjectPtr<UWorld>& Map, ECouchGameFlowState StateForLog)
{
	const FString PackageName = Map.ToSoftObjectPath().GetLongPackageName();
	if (PackageName.IsEmpty())
	{
		UE_LOG(LogCouchGameFlow, Warning, TEXT("No map configured for flow state %s"), *UEnum::GetValueAsString(StateForLog));
		return;
	}
	UGameplayStatics::OpenLevel(this, FName(*PackageName));
}

bool UCouchGameFlowSubsystem::ExecuteTransition(ECouchGameFlowState NewState, ETransitionExecutionMode Mode)
{
	if (Mode == ETransitionExecutionMode::Normal && !IsTransitionAllowed(CurrentState, NewState))
	{
		UE_LOG(LogCouchGameFlow, Warning, TEXT("Rejected flow transition : %s -> %s"), *UEnum::GetValueAsString(CurrentState), *UEnum::GetValueAsString(NewState));
		return false;
	}

	const ECouchGameFlowState OldState = CurrentState;
	CurrentState = NewState;
	UE_LOG(LogCouchGameFlow, Warning, TEXT("Changed state : %s -> %s"), *UEnum::GetValueAsString(OldState), *UEnum::GetValueAsString(NewState));
	OnGameFlowStateChanged.Broadcast(OldState, NewState);
	
	return true;
}

//Good practice request during Gameplay, so it send a normal execution mode to the execute transition. This can be skipped if you want to execute the transition directly.
bool UCouchGameFlowSubsystem::RequestTransition(ECouchGameFlowState NewState)
{
	return ExecuteTransition(NewState, ETransitionExecutionMode::Normal);
}

//Called in a AProtoGameMode, this is made so some Gamemodes that don't start at main menu simply start at gameplay
void UCouchGameFlowSubsystem::OnGameModeBeginPlay(AProtoGameMode* GM)
{
	if (!GM->StartWithMainMenu || GM->bConnectOnEditor)
	{
		ExecuteTransition(ECouchGameFlowState::Gameplay, ETransitionExecutionMode::DirectGameplay);
	}
}


