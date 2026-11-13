// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Engine/DeveloperSettings.h"
#include "CouchDevSettings.generated.h"

class UWorld;

UENUM(BlueprintType)
enum class ECouchInputScheme : uint8
{
	KeyboardA,
	KeyboardB,
	Gamepad_Xbox
};


USTRUCT(BlueprintType)
struct FCouchInputSchemeMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Input")
	ECouchInputScheme Scheme = ECouchInputScheme::KeyboardA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> MappingContext;

	// UI metadata only. Used for the AI to know where to pick up the icons with names using a data table
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Input")
	TArray<FString> InputIconNames;
};


/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Couch Game Settings"))
class COUCHGAME_API UCouchDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UCouchDevSettings();

	bool TryGetInputSchemeMapping(ECouchInputScheme Scheme, FCouchInputSchemeMapping& OutMapping) const;
	
	UPROPERTY(EditAnywhere, Config, Category = "Input Schemes")
	TArray<FCouchInputSchemeMapping> InputSchemeMappings;
};
