// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtoCharacter.h"
#include "LifeComponent.h"


// Sets default values
AProtoCharacter::AProtoCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LifeComponent = CreateDefaultSubobject<ULifeComponent>(TEXT("LifeComponent"));
}

// Called when the game starts or when spawned
void AProtoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProtoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AProtoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


bool AProtoCharacter::TryCollectCurrency_Implementation(const int32 Amount)
{
	if (Amount <= 0 || IsDeadOrMissingLife())
	{
		return false;
	}
	AProtoPlayerState* CouchState = GetCouchPlayerState();
	return CouchState && CouchState->TryAddCurrency(Amount);
}


bool AProtoCharacter::TryRemoveCurrency_Implementation(const int32 Amount)
{
	if (Amount <= 0 || IsDeadOrMissingLife())
	{
		return false;
	}
	AProtoPlayerState* CouchState = GetCouchPlayerState();
	return CouchState && CouchState->TryRemoveCurrency(Amount) > 0;
}


//When called, tries to make the life component take damage
bool AProtoCharacter::TryTakeDamage_Implementation(const float Damage, float RagdollPushForce, const FHitResult HitResult)
{
	if (Damage <= 0.0f || IsDeadOrMissingLife())
	{
		return false;
	}
	
	AProtoPlayerState* CouchState = GetCouchPlayerState();
	LifeComponent->ApplyDamage(Damage, nullptr, nullptr, RagdollPushForce, HitResult);
	if (IsDeadOrMissingLife() && CouchState)
	{
		CouchState->IncrementDeathCount();
	}
	return true;
}


// Return the cached Couch state, refreshing it when PlayerState becomes available or changes.
AProtoPlayerState* AProtoCharacter::GetCouchPlayerState() const
{
	AProtoPlayerState* CurrentCouchPlayerState = GetPlayerState<AProtoPlayerState>();
	if (CachedCouchPlayerState != CurrentCouchPlayerState)
	{
		CachedCouchPlayerState = CurrentCouchPlayerState;
	}
	return CachedCouchPlayerState;
}

int32 AProtoCharacter::GetCouchPlayerIndex() const
{
	if (const AProtoPlayerState* CouchState = GetCouchPlayerState())
	{
		return CouchState->GetCouchPlayerIndex();
	}
	return INDEX_NONE;
}

bool AProtoCharacter::IsDeadOrMissingLife() const
{
	return !LifeComponent || LifeComponent->IsDead();
}