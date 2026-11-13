// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ProtoPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COUCHGAME_API AProtoPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Couch|Player")
	int32 GetCouchPlayerIndex() const { return CouchPlayerIndex; }
	
	void SetCouchPlayerIndex(int32 InCouchPlayerIndex) { CouchPlayerIndex = InCouchPlayerIndex; }
	
	void ResetCurrency(int32 NewBalance = 0);

	UFUNCTION(BlueprintPure, Category = "Couch|Currency")
	int32 GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintPure, Category = "Couch|Player")
	int32 GetDeathCount() const { return DeathCount; }

	void IncrementDeathCount();
	
	// Adds a positive amount of Currency and reports whether the balance changed.
	UFUNCTION(BlueprintCallable, Category = "Couch|Currency")
	bool TryAddCurrency(int32 Amount);

	// Removes up to the requested positive amount and returns the number of Currency actually removed.
	UFUNCTION(BlueprintCallable, Category = "Couch|Currency")
	int32 TryRemoveCurrency(int32 Amount);
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Couch|Player", meta = (AllowPrivateAccess = "true", ToolTip = "Couch player index assigned to this player state."))
	int32 CouchPlayerIndex = INDEX_NONE;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Couch|Currency", meta = (AllowPrivateAccess = "true", ToolTip = "Current non-negative Currency balance."))
	int32 Currency = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Couch|Player", meta = (AllowPrivateAccess = "true", ToolTip = "Number of times this player has died."))
	int32 DeathCount = 0;
};
