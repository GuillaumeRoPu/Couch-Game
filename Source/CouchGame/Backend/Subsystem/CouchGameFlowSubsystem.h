// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Backend/CouchDevSettings.h" 
#include "CouchLobbySubsystem.h"
#include "CouchGameFlowSubsystem.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogCouchGameFlow, Log, All);

UENUM(BlueprintType)
enum class ECouchGameFlowState : uint8
{
	None,
	MainMenu,
	Lobby,
	Gameplay,
	FinalScreen,
};

UENUM()
enum class ETransitionExecutionMode : uint8
{
	Normal,
	DirectPIE,
	SessionReset,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCouchGameFlowStateChanged, ECouchGameFlowState, OldState, ECouchGameFlowState, NewState);

/**
 * 
 */
UCLASS()
class COUCHGAME_API UCouchGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Couch|GameFlow")
	FOnCouchGameFlowStateChanged OnGameFlowStateChanged;

	UFUNCTION(BlueprintPure, Category = "Couch|GameFlow")
	ECouchGameFlowState GetCurrentState() const { return CurrentState; }
	
	bool RequestTransition(ECouchGameFlowState NewState);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	ECouchGameFlowState CurrentState = ECouchGameFlowState::None;

	UPROPERTY()
	TObjectPtr<UCouchLobbySubsystem> LobbySubsystem;

	const UCouchDevSettings* DevSettings = nullptr;
	
	bool IsTransitionAllowed(ECouchGameFlowState From, ECouchGameFlowState To) const;
	
	bool ExecuteTransition(ECouchGameFlowState NewState, ETransitionExecutionMode Mode);
	void OpenConfiguredMap(const TSoftObjectPtr<UWorld>& Map, ECouchGameFlowState StateForLog);

};
