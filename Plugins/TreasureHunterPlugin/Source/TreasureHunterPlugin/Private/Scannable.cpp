// Fill out your copyright notice in the Description page of Project Settings.

#include "TreasureHunter.h"
#include "Scannable.h"


// Sets default values
AScannable::AScannable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//All scannable object start as unsanned
	bIsScanned = false;

	//Create the static mesh component
	ScannableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScannableMesh"));
	RootComponent = ScannableMesh;

}

// Called when the game starts or when spawned
void AScannable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScannable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AScannable::IsScanned()
{
	return bIsScanned;
}

void AScannable::SetScanned(bool newScannableState)
{
	bIsScanned = newScannableState;
}

void AScannable::WasScanned_Implementation()
{
	// Log a debug message
	FString ScannedDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You have scanned %s"), *ScannedDebugString);

}