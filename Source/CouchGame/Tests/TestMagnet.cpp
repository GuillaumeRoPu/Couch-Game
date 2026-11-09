// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/TestMagnet.h"

// Sets default values for this component's properties
UTestMagnet::UTestMagnet()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTestMagnet::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTestMagnet::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

