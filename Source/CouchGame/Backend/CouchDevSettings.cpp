// Fill out your copyright notice in the Description page of Project Settings.


#include "CouchDevSettings.h"

UCouchDevSettings::UCouchDevSettings()
{
	CategoryName = FName(TEXT("Game"));
}

bool UCouchDevSettings::TryGetInputSchemeMapping(ECouchInputScheme Scheme, FCouchInputSchemeMapping& OutMapping) const
{
	for (const FCouchInputSchemeMapping& Mapping : InputSchemeMappings)
	{
		if (Mapping.Scheme == Scheme)
		{
			OutMapping = Mapping;
			return true;
		}
	}
	return false;
}
