// Fill out your copyright notice in the Description page of Project Settings.

#include "TreasureHunter.h"
#include "ScannableComponent.h"


// Sets default values for this component's properties
UScannableComponent::UScannableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UScannableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UScannableComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

bool UScannableComponent::IsScanned()
{
	return bIsScanned;
}

void UScannableComponent::SetScanned(bool newScannableState)
{
	bIsScanned = newScannableState;
}

void UScannableComponent::WasScanned_Implementation()
{
	// Log a debug message
	FString ScannedDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You have scanned %s"), *ScannedDebugString);
}