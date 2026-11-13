// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Backend/CouchDevSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CouchLobbySubsystem.generated.h"

// Player Info for the entire game, if you need to stock info about a player that needs to stay in every level, it is done here
USTRUCT(BlueprintType)
struct COUCHGAME_API FCouchPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Couch|Player")
	int32 CouchPlayerIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Couch|Player")
	ECouchInputScheme Scheme = ECouchInputScheme::KeyboardA;

	UPROPERTY(BlueprintReadOnly, Category = "Couch|Player")
	FCouchInputSchemeMapping SchemeMapping;

	UPROPERTY(BlueprintReadOnly, Category = "Couch|Player")
	FKey TriggerKey;

	UPROPERTY(BlueprintReadOnly, Category = "Couch|Player")
	int32 DeviceId = INDEX_NONE;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnCouchPlayerJoined,
	int32, CouchPlayerIndex,
	ECouchInputScheme, Scheme,
	FCouchInputSchemeMapping, SchemeMapping,
	FKey, TriggerKey,
	int32, DeviceId);



/**
 * 
 */
UCLASS()
class COUCHGAME_API UCouchLobbySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	static constexpr int32 MaximumCouchPlayers = 2;

	TSet<int32> ClaimedPlayerIndices;
	TSet<ECouchInputScheme> ClaimedKeyboardSchemes;
	bool bJoiningOpen = false;
	
	void ClaimPlayerIndex(int32 CouchPlayerIndex, ECouchInputScheme Scheme, const FKey& Key, int32 DeviceId);
	int32 FindFreeCouchPlayerIndex() const;
	bool TryClaimInputSource(ECouchInputScheme Scheme, const FKey& Key, int32 DeviceId, int32& OutCouchPlayerIndex);
	
	void ApplyInputMappingForPlayerIndex(int32 PartyPlayerIndex, ULocalPlayer* LocalPlayer);

	UPROPERTY(Transient)
	TMap<int32, TWeakObjectPtr<ULocalPlayer>> PlayerIndexLocalPlayers;

	
public:
	bool NotifyKeyboardSchemeInput(ECouchInputScheme Scheme, const FKey& Key, FInputDeviceId Device);
	
	APlayerController* GetPlayerControllerForScheme(ECouchInputScheme Scheme) const;
	
	bool ClaimNextUnassignedLocalPlayer(ULocalPlayer* LocalPlayer, int32& OutPartyPlayerIndex);
	APlayerController* GetPlayerControllerForPlayerIndex(int32 PartyPlayerIndex) const;


	UFUNCTION(BlueprintCallable, Category = "Couch|Lobby")
	void OpenJoining();

	UFUNCTION(BlueprintCallable, Category = "Couch|Lobby")
	void CloseJoining();

	UFUNCTION(BlueprintPure, Category = "Couch|Lobby")
	bool IsJoiningOpen() const;

	UFUNCTION(BlueprintPure, Category = "Couch|Lobby")
	bool IsPlayerIndexClaimed(int32 CouchPlayerIndex) const;

	UFUNCTION(BlueprintPure, Category = "Couch|Lobby")
	int32 GetJoinedPlayerCount() const;
	
	UPROPERTY()
	TMap<int32, FCouchPlayerInfo> JoinedPlayers;

	UPROPERTY(BlueprintAssignable, Category = "Couch|Lobby")
	FOnCouchPlayerJoined OnCouchPlayerJoined;
};
