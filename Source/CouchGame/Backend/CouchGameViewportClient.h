// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "Subsystem/CouchLobbySubsystem.h"
#include "CouchGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class COUCHGAME_API UCouchGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	virtual void Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice) override;
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual bool InputAxis(const FInputKeyEventArgs& EventArgs) override;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UCouchLobbySubsystem> LobbySubsystem = nullptr;
	
	static constexpr int32 KeyboardSchemeCount = 2;
	TArray<TSet<FKey>> SchemeKeys;
	bool bSchemeKeysBuilt = false;

	void EnsureSchemeKeysBuilt();
	static bool IsKeyboardRoutingKey(const FKey& Key);
	static bool RouteInputToController(APlayerController* PlayerController, const FInputKeyEventArgs& EventArgs);
	
	bool RouteGamepadInput(const FInputKeyEventArgs& EventArgs, bool bNotifyUnclaimedDevice) const;
	bool RouteKeyboardInput(const FInputKeyEventArgs& EventArgs) const;
};
