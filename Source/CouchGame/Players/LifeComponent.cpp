// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/LifeComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values for this component's properties
ULifeComponent::ULifeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULifeComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentLifePoints = GetNormalizedMaxLifePoints();
}

void ULifeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void ULifeComponent::ApplyDamage(float Damage, AController* Instigator, AActor* DamageCauser, const float RagdollPushForce, const FHitResult& HitResult)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}
	CurrentLifePoints = FMath::Clamp(CurrentLifePoints - Damage, 0.0f, GetNormalizedMaxLifePoints());
	OnDamageTaken.Broadcast(Damage, CurrentLifePoints, Instigator, DamageCauser);
	if (CurrentLifePoints <= 0.0f)
	{
		bIsDead = true;
		ApplyDeath(Instigator, DamageCauser, RagdollPushForce, FCouchDamageContext{Instigator, DamageCauser, HitResult.ImpactPoint, HitResult.ImpactNormal});
	}
}

float ULifeComponent::GetNormalizedMaxLifePoints() const
{
	return FMath::Max(MinimumLifePoints, MaxLifePoints);
}


// Applies the character death presentation before notifying Blueprint and other listeners.
void ULifeComponent::ApplyDeath(AController* Instigator, AActor* DamageCauser, const float RagdollPushForce, const FCouchDamageContext& Context)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		PlayDeathAudio(Character);
		ApplyRagdoll(Character, Context, RagdollPushForce);
	}
	OnDied.Broadcast(Character, Instigator, DamageCauser, Context);
}

void ULifeComponent::PlayDeathAudio(const ACharacter* Character) const
{
	if (DeathSFX && GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSFX, Character->GetActorLocation(), 1.0f, FMath::FRandRange(0.8f, 1.2f));
	}
}

void ULifeComponent::ApplyRagdoll(ACharacter* Character, const FCouchDamageContext& Context, const float RagdollPushForce) const
{
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->DisableMovement();
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	// UE: physics simulation requires a collision mode that supports physics bodies.
	Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();
	if (!Context.ImpactNormal.IsNearlyZero())
	{
		const FVector Impulse = -Context.ImpactNormal * RagdollPushForce;
		const FVector ImpactPoint = Context.ImpactPoint.IsNearlyZero()
			? Character->GetActorLocation()
			: Context.ImpactPoint;
		Mesh->AddImpulseToAllBodiesBelow(Impulse, Mesh->GetBoneName(0), false, true);
	}
}

// Restores life and reverses the movement, collision, and physics changes made by death.
void ULifeComponent::ResetLife()
{
	bIsDead = false;
	CurrentLifePoints = GetNormalizedMaxLifePoints();
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		Mesh->SetAllBodiesSimulatePhysics(false);
		Mesh->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	OnLifeReset.Broadcast(Character, CurrentLifePoints);
}

void ULifeComponent::RetireForRespawn(const float RetireDelay)
{
	AActor* Owner = GetOwner();
	if (!bIsDead || !Owner)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		// Keep simulated bodies active; NoCollision is incompatible with a live ragdoll.
		Character->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	if (RetireDelay > 0.0f)
	{
		Owner->SetLifeSpan(RetireDelay);
	}
	else
	{
		Owner->Destroy();
	}
}