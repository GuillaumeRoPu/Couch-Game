// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoPlayerState.h"

// Resets the coin balance while preventing a negative starting balance.
void AProtoPlayerState::ResetCurrency(const int32 NewBalance)
{
	Currency = FMath::Max(0, NewBalance);
}

void AProtoPlayerState::IncrementDeathCount()
{
	++DeathCount;
}

// Adds Currency only when the requested amount is positive.
bool AProtoPlayerState::TryAddCurrency(const int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}
	Currency += Amount;
	return true;
}

// Removes only the amount available, returning the actual deduction.
int32 AProtoPlayerState::TryRemoveCurrency(const int32 Amount)
{
	if (Amount <= 0)
	{
		return 0;
	}

	const int32 CurrencyRemoved = FMath::Min(Amount, FMath::Max(0, Currency));
	Currency -= CurrencyRemoved;
	return CurrencyRemoved;
}