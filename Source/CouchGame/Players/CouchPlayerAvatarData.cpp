// Fill out your copyright notice in the Description page of Project Settings.


#include "CouchPlayerAvatarData.h"

UTexture2D* UCouchPlayerAvatarData::LoadIconTexture() const
{
	// UE: resolving a soft reference synchronously can block, so call this from UI setup/loading paths.
	return Icon.LoadSynchronous();
}

USkeletalMesh* UCouchPlayerAvatarData::LoadSkeletalMesh() const
{
	// UE: resolving a soft reference synchronously can block, so call this during character setup/loading paths.
	return SkeletalMesh.LoadSynchronous();
}

UAnimBlueprint* UCouchPlayerAvatarData::LoadAnimationBlueprint() const
{
	return AnimationBlueprintLibrary.LoadSynchronous();
}
