// Fill out your copyright notice in the Description page of Project Settings.


#include "CouchGameViewportClient.h"

// Override. Called on initialization of the game. Here it is just used to store the Lobby Subsystem. If needed afterward, we can make it remember other subsystems. Don't use it to store data that is destroyed during gameplay like levels and stuff since it might break stuff later on.
void UCouchGameViewportClient::Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	LobbySubsystem = OwningGameInstance ? OwningGameInstance->GetSubsystem<UCouchLobbySubsystem>() : nullptr;
	
	EnsureSchemeKeysBuilt();
}

// Override. Called on EVERY Key input. Here, we filter through all inputs to find only the ones we want from the schemes we implemented in dev settings. Once they're filtered, we send the appropriate schemes to the lobby so each scheme can get claimed.
bool UCouchGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	EnsureSchemeKeysBuilt();
	if (RouteKeyboardInput(EventArgs)) return true;
	if (EventArgs.Key.IsGamepadKey() && EventArgs.Key.IsButtonAxis() && RouteGamepadInput(EventArgs, true)) return true;
	return Super::InputKey(EventArgs);
}

// Override. Called on EVERY Axis input. Here, we filter through all inputs to find only the ones we want from the schemes we implemented in dev settings.
bool UCouchGameViewportClient::InputAxis(const FInputKeyEventArgs& EventArgs)
{
	if (EventArgs.Key.IsGamepadKey() && RouteGamepadInput(EventArgs, false)) return true;
	return Super::InputAxis(EventArgs);
}

// Tells you if the key is a keyboard button (like A, &, (, R, shift, etc etc)
bool UCouchGameViewportClient::IsKeyboardRoutingKey(const FKey& Key)
{
	return Key.IsValid() && Key.IsBindableToActions() && Key.IsDigital()
		&& !Key.IsGamepadKey() && !Key.IsMouseButton() && !Key.IsTouch()
		&& !Key.IsAxis1D() && !Key.IsAxis2D() && !Key.IsAxis3D() && !Key.IsGesture();
}

// When given a player controller, says to said player controller that an input (here EventArgs) has been done so it can act (or not) on this input
bool UCouchGameViewportClient::RouteInputToController(APlayerController* PlayerController, const FInputKeyEventArgs& EventArgs)
{
	if (!PlayerController) return true;
	FInputKeyEventArgs RoutedEventArgs = EventArgs;
	if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		RoutedEventArgs.ControllerId = LocalPlayer->GetControllerId();
	}
	PlayerController->InputKey(RoutedEventArgs);
	return true;
}

//If the key EventArgs is inside a scheme at SchemeIndex, it routes the input to the controller for the scheme
bool UCouchGameViewportClient::RouteKeyboardInput(const FInputKeyEventArgs& EventArgs) const
{
	for (int32 SchemeIndex = 0; SchemeIndex < KeyboardSchemeCount; ++SchemeIndex)
	{
		if (!SchemeKeys[SchemeIndex].Contains(EventArgs.Key)) continue;

		const ECouchInputScheme Scheme = static_cast<ECouchInputScheme>(SchemeIndex);
		if (!LobbySubsystem) return false;

		if (APlayerController* PlayerController = LobbySubsystem->GetPlayerControllerForScheme(Scheme))
		{
			if (!LobbySubsystem->IsJoiningOpen())
			{
				return RouteInputToController(PlayerController, EventArgs);
			}
		}

		return LobbySubsystem->NotifyKeyboardSchemeInput(Scheme, EventArgs.Key, EventArgs.InputDevice);
	}

	return false;
}

//Call this whenever you intend to use SchemeKeys to check something. This method is used to add all keyboard button keys from each Scheme from the dev settings inside SchemeKeys, which will be used later to know whether a keyboard button pressed is owned by one scheme or another. I commented everything for a complete breakthrough cuz it's hard to read.
void UCouchGameViewportClient::EnsureSchemeKeysBuilt()
{
	//If this has already been called its skipped entirely (since SchemeKeys already has all the info)
	if (bSchemeKeysBuilt) return;
	SchemeKeys.SetNum(KeyboardSchemeCount);
	for (TSet<FKey>& Keys : SchemeKeys) Keys.Reset();

	//Created here, used at the end to check if 2 mappings have the same input, and prioritizes the first mapping to filter duplicate keys (so 2 mappings can't get claimed at once)
	TMap<FKey, ECouchInputScheme> KeyOwners;
	const UCouchDevSettings* Settings = GetDefault<UCouchDevSettings>();
	//This for loop checks every single SchemeMapping, and if it meets the conditions, it adds all of its keyboard keys to SchemeKeys.
	for (const FCouchInputSchemeMapping& SchemeMapping : Settings->InputSchemeMappings)
	{
		const int32 SchemeIndex = static_cast<int32>(SchemeMapping.Scheme);
		if (SchemeIndex < 0 || SchemeIndex >= KeyboardSchemeCount) continue;

		const UInputMappingContext* MappingContext = SchemeMapping.MappingContext.LoadSynchronous();
		if (!MappingContext) continue;

		for (const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
		{
			const FKey& Key = Mapping.Key;
			if (!IsKeyboardRoutingKey(Key)) continue;
			if (KeyOwners.Contains(Key)) continue;
			KeyOwners.Add(Key, SchemeMapping.Scheme);
			SchemeKeys[SchemeIndex].Add(Key);
		}
	}
	bSchemeKeysBuilt = true;
}

//Same thing as RouteKeyboardInput but for gamepads. We separate the 2 of them since gamepads aren't stored the same way as keyboard schemes.
bool UCouchGameViewportClient::RouteGamepadInput(const FInputKeyEventArgs& EventArgs, bool bNotifyUnclaimedDevice) const
{
	if (LobbySubsystem)
	{
		if (APlayerController* PlayerController = LobbySubsystem->GetPlayerControllerForGamepadDevice(EventArgs.InputDevice.GetId()))
		{
			return RouteInputToController(PlayerController, EventArgs);
		}

		if (bNotifyUnclaimedDevice)
		{
			LobbySubsystem->NotifyGamepadInput(EventArgs.Key, EventArgs.InputDevice);
		}
	}

	return true;
}