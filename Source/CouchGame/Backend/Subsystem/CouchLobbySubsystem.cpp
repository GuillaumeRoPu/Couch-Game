// Fill out your copyright notice in the Description page of Project Settings.


#include "CouchLobbySubsystem.h"

#include "EnhancedInputSubsystems.h"


DEFINE_LOG_CATEGORY_STATIC(LogCouchLobby, Log, All);

// Returns the first index of player that still isn't claimed by an input scheme yet. Returns INDEX_NONE if it can't find any (only happens if the list is full aka max number of players attained).
int32 UCouchLobbySubsystem::FindFreeCouchPlayerIndex() const
{
	for (int32 CouchPlayerIndex = 0; CouchPlayerIndex < MaximumCouchPlayers; ++CouchPlayerIndex)
	{
		if (!ClaimedPlayerIndices.Contains(CouchPlayerIndex))
		{
			return CouchPlayerIndex;
		}
	}
	
	return INDEX_NONE;
}

// Whenever it is called, it tries to claim the scheme at a free player index. Only works if joining is open and if the player limit hasn't been attained yet.
bool UCouchLobbySubsystem::TryClaimInputSource(ECouchInputScheme Scheme, const FKey& Key, int32 DeviceId, int32& OutCouchPlayerIndex)
{
	OutCouchPlayerIndex = FindFreeCouchPlayerIndex();
	if (!bJoiningOpen || OutCouchPlayerIndex == INDEX_NONE)
	{
		return false;
	}
	
	ClaimPlayerIndex(OutCouchPlayerIndex, Scheme, Key, DeviceId);
	
	return true;
}

// Whenever a keyboard key is pressed that doesn't already belong to a claimed scheme, this is called to notify the system to try to claim the input to a player. If this keyboard scheme is already used, it doesn't claim anything since it's already used.
bool UCouchLobbySubsystem::NotifyKeyboardSchemeInput(ECouchInputScheme Scheme, const FKey& Key, FInputDeviceId Device)
{
	if (ClaimedKeyboardSchemes.Contains(Scheme))
	{
		return true;
	}
	
	int32 ClaimedPlayerIndex = INDEX_NONE;
	if (TryClaimInputSource(Scheme, Key, Device.GetId(), ClaimedPlayerIndex))
	{
		ClaimedKeyboardSchemes.Add(Scheme);
	}
	
	return true;
}

//Whenever this is called, it gives you the player controller for the given scheme (mainly used for keyboard since it's the only device with multiple different schemes)
APlayerController* UCouchLobbySubsystem::GetPlayerControllerForScheme(ECouchInputScheme Scheme) const
{
	for (const TPair<int32, FCouchPlayerInfo>& Pair : JoinedPlayers)
	{
		if (Pair.Value.Scheme == Scheme)
		{
			return GetPlayerControllerForPlayerIndex(Pair.Key);
		}
	}

	return nullptr;
}

//Whenever it is called, it claims the next unassigned LocalPlayer, asks to apply a mapping context to the local player and gives the player index back for said local player. It is also stocked in PlayerIndexLocalPlayers
bool UCouchLobbySubsystem::ClaimNextUnassignedLocalPlayer(ULocalPlayer* LocalPlayer, int32& OutCouchPlayerIndex)
{
	OutCouchPlayerIndex = INDEX_NONE;
	if (!LocalPlayer)
	{
		return false;
	}

	for (int32 CouchPlayerIndex = 0; CouchPlayerIndex < MaximumCouchPlayers; ++CouchPlayerIndex)
	{
		if (JoinedPlayers.Contains(CouchPlayerIndex) && !PlayerIndexLocalPlayers.Contains(CouchPlayerIndex))
		{
			PlayerIndexLocalPlayers.Add(CouchPlayerIndex, LocalPlayer);
			ApplyInputMappingForPlayerIndex(CouchPlayerIndex, LocalPlayer);
			OutCouchPlayerIndex = CouchPlayerIndex;
			return true;
		}
	}

	return false;
}

//Whenever this is called, it searches for the mapping connected to the player info connected to the couch player index given, and then connects said context to the LocalPlayer given
void UCouchLobbySubsystem::ApplyInputMappingForPlayerIndex(int32 CouchPlayerIndex, ULocalPlayer* LocalPlayer)
{
	const FCouchPlayerInfo* PlayerInfo = JoinedPlayers.Find(CouchPlayerIndex);
	if (!PlayerInfo || !LocalPlayer)
	{
		return;
	}

	UInputMappingContext* MappingContext = PlayerInfo->SchemeMapping.MappingContext.LoadSynchronous();
	if (!MappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->AddMappingContext(MappingContext, 0);
	}
}

//Gives you the player controller whenever you give a player index (PlayerIndexLocalPlayers stocks every LocalPlayer with a key being their index)
APlayerController* UCouchLobbySubsystem::GetPlayerControllerForPlayerIndex(int32 CouchPlayerIndex) const
{
	const TWeakObjectPtr<ULocalPlayer>* LocalPlayer = PlayerIndexLocalPlayers.Find(CouchPlayerIndex);
	if (LocalPlayer && LocalPlayer->IsValid())
	{
		return LocalPlayer->Get()->GetPlayerController(GetWorld());
	}

	return nullptr;
}


//Opens joining the lobby for players
void UCouchLobbySubsystem::OpenJoining()
{
	bJoiningOpen = true;
	UE_LOG(LogCouchLobby, Log, TEXT("Joining opened."));
}

//Closes joining the lobby for players
void UCouchLobbySubsystem::CloseJoining()
{
	bJoiningOpen = false;
	UE_LOG(LogCouchLobby, Log, TEXT("Joining closed."));
}

//Returns whether you are able to join the lobby or not
bool UCouchLobbySubsystem::IsJoiningOpen() const
{
	return bJoiningOpen;
}

//Returns whether the player index is already claimed
bool UCouchLobbySubsystem::IsPlayerIndexClaimed(int32 CouchPlayerIndex) const
{
	return ClaimedPlayerIndices.Contains(CouchPlayerIndex);	
}

//Returns the number of players that are claimed
int32 UCouchLobbySubsystem::GetJoinedPlayerCount() const
{
	return ClaimedPlayerIndices.Num();
}

//Claims the player at given index, then sets its player info based on given info to store it into JoinedPlayers, then broadcasts OnCouchPlayerJoined. Only use on its own when you know what you're doing.
void UCouchLobbySubsystem::ClaimPlayerIndex(int32 CouchPlayerIndex, ECouchInputScheme Scheme, const FKey& Key, int32 DeviceId)
{
	ClaimedPlayerIndices.Add(CouchPlayerIndex);

	FCouchInputSchemeMapping SchemeMapping;
	GetDefault<UCouchDevSettings>()->TryGetInputSchemeMapping(Scheme, SchemeMapping);

	FCouchPlayerInfo PlayerInfo;
	PlayerInfo.CouchPlayerIndex = CouchPlayerIndex;
	PlayerInfo.Scheme = Scheme;
	PlayerInfo.SchemeMapping = SchemeMapping;
	PlayerInfo.TriggerKey = Key;
	PlayerInfo.DeviceId = DeviceId;
	JoinedPlayers.Add(CouchPlayerIndex, PlayerInfo);

	UE_LOG(LogCouchLobby, Log, TEXT("Player joined - CouchPlayerIndex=%d, Scheme=%d, Key=%s, DeviceId=%d"), CouchPlayerIndex, static_cast<int32>(Scheme), *Key.ToString(), DeviceId);

	OnCouchPlayerJoined.Broadcast(CouchPlayerIndex, Scheme, SchemeMapping, Key, DeviceId);
}

//Whenever this is called, it claims the input source with the DeviceId to differentiate each gamepad from one another
bool UCouchLobbySubsystem::NotifyGamepadInput(const FKey& Key, FInputDeviceId Device)
{
	const int32 DeviceId = Device.GetId();
	if (GamepadDeviceToPlayerIndex.Contains(DeviceId))
	{
		return false;
	}

	int32 ClaimedPlayerIndex = INDEX_NONE;
	if (TryClaimInputSource(ECouchInputScheme::Gamepad_Xbox, Key, DeviceId, ClaimedPlayerIndex))
	{
		GamepadDeviceToPlayerIndex.Add(DeviceId, ClaimedPlayerIndex);
	}

	return false;
}

//Gets the player controller connected with the given deviceId if it is found in GamepadDeviceToPlayerIndex
APlayerController* UCouchLobbySubsystem::GetPlayerControllerForGamepadDevice(int32 DeviceId) const
{
	const int32* CouchPlayerIndex = GamepadDeviceToPlayerIndex.Find(DeviceId);
	if (CouchPlayerIndex)
	{
		return GetPlayerControllerForPlayerIndex(*CouchPlayerIndex);
	}

	return nullptr;
}