// Fill out your copyright notice in the Description page of Project Settings.

#include "TreasureHunter.h"
#include "ScanComponent.h"
#include "ScannableComponent.h"
#include "Scannable.h"


// Sets default values for this component's properties
UScanComponent::UScanComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	//Create a scanning sphere
	ScanningSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ScanningSphere"));
	//ScanningSphere->AttachTo(GetOwner()->GetRootComponent()); ////////////////////////////////////////////////////////////////////////////////////////////
	
	//Create 4 modes
	for (int32 i = 0; i < 4; i++)
	{
		ModesQueue.Enqueue(i);
	}

	ScanningSphere->SetSphereRadius(300.f);
	
	// ...
}



// Called when the game starts
void UScanComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UScanComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}


// Scan implementation
void UScanComponent::Scan()
{
	//get overlapping actors and store them in array
	TArray<AActor*> ScannedActors;
	ScanningSphere->GetOverlappingActors(ScannedActors);

	//Get current mode of scanning
	int32 CurrentMode;
	ModesQueue.Peek(CurrentMode);

	switch (CurrentMode)
	{
	case 0:	ScanMode0(ScannedActors);	break;
	case 1: ScanMode1(ScannedActors);	break;
	case 2: ScanMode2(ScannedActors);	break;
	case 3: ScanMode3(ScannedActors);	break;
	default: ScanMode0(ScannedActors);
	}
}

void UScanComponent::ScanMode0(TArray<AActor*> ScannedActors)
{
	//If actor is overlapping with scanning sphere, mark actor as scanned
	for (int32 iScanned = 0; iScanned < ScannedActors.Num(); ++iScanned)
	{
		AScannable* const TestScan = Cast<AScannable>(ScannedActors[iScanned]);
		if (TestScan && !TestScan->IsPendingKill() && !TestScan->IsScanned())
		{
			TestScan->WasScanned();
			TestScan->SetScanned(true);
		}
		else
		{
			//Search for UScannableComponent in actor
			UActorComponent* tempPointer = ScannedActors[iScanned]->GetComponentByClass(UScannableComponent::StaticClass());
			//Casting UActorComponent* to UscannableComponent*
			UScannableComponent *ScannablePointer = static_cast<UScannableComponent*>(tempPointer);

			//If UScannableComponent is found, mark component as scanned
			if (tempPointer != NULL && !ScannablePointer->IsScanned())
			{
				ScannablePointer->WasScanned();
				ScannablePointer->SetScanned(true);
			}
		}
	}
}

void UScanComponent::ScanMode1(TArray<AActor*> ScannedActors)
{
	//If actor is overlapping with scanning sphere, mark actor as scanned
	for (int32 iScanned = 0; iScanned < ScannedActors.Num(); ++iScanned)
	{
		AScannable* TestScan = Cast<AScannable>(ScannedActors[iScanned]);
		if (TestScan && !TestScan->IsPendingKill() && !TestScan->IsScanned())
		{
			TestScan->WasScanned();
			TestScan->SetScanned(true);
			TestScan->Destroy();
		}
		else
		{
			//Search for UScannableComponent in actor
			UActorComponent* tempPointer = ScannedActors[iScanned]->GetComponentByClass(UScannableComponent::StaticClass());
			//Casting UActorComponent* to UscannableComponent*
			UScannableComponent *ScannablePointer = static_cast<UScannableComponent*>(tempPointer);

			//If UScannableComponent is found, mark component as scanned and destroy it
			if (tempPointer != NULL && !ScannablePointer->IsScanned())
			{
				ScannablePointer->WasScanned();
				ScannablePointer->SetScanned(true);
				tempPointer = NULL;
				ScannablePointer = NULL;
				ScannedActors[iScanned]->Destroy();
			}
		}
	}
}

void UScanComponent::ScanMode2(TArray<AActor*> ScannedActors)
{
	//Set ScanningSphere to bigger radius
	ScanningSphere->SetSphereRadius(1000);
	//If actor is overlapping with scanning sphere, mark actor as scanned
	for (int32 iScanned = 0; iScanned < ScannedActors.Num(); ++iScanned)
	{
		AScannable* const TestScan = Cast<AScannable>(ScannedActors[iScanned]);
		if (TestScan && !TestScan->IsPendingKill() && !TestScan->IsScanned())
		{
			//Get location
			FVector CurrentLocation = TestScan->GetActorLocation();
			float speed = 15000.0f;
			//Set position
			CurrentLocation.Z += speed * GetWorld()->GetDeltaSeconds();
			TestScan->SetActorRelativeLocation(CurrentLocation);
		}
		else
		{
			//Search for UScannableComponent in actor
			UActorComponent* tempPointer = ScannedActors[iScanned]->GetComponentByClass(UScannableComponent::StaticClass());
			//Casting UActorComponent* to UscannableComponent*
			UScannableComponent *ScannablePointer = static_cast<UScannableComponent*>(tempPointer);

			//If UScannableComponent is found, mark component as scanned
			if (tempPointer != NULL && !ScannablePointer->IsScanned())
			{
				FVector CurrentLocation = ScannedActors[iScanned]->GetActorLocation();
				float speed = 15000.0f;
				CurrentLocation.Z += speed * GetWorld()->GetDeltaSeconds();
				ScannedActors[iScanned]->SetActorRelativeLocation(CurrentLocation);
			}
		}
	}
	//Return ScanningSphere to original size
	ScanningSphere->SetSphereRadius(300);
}

void UScanComponent::ScanMode3(TArray<AActor*> ScannedActors)
{
	//If actor is overlapping with scanning sphere, mark actor as scanned
	for (int32 iScanned = 0; iScanned < ScannedActors.Num(); ++iScanned)
	{
		AScannable* const TestScan = Cast<AScannable>(ScannedActors[iScanned]);
		if (TestScan && !TestScan->IsPendingKill() && !TestScan->IsScanned())
		{
			//get distance to scannable object and log it
			//float distance = this->GetHorizontalDistanceTo(TestScan);
			//UE_LOG(LogClass, Log, TEXT("Distance to scannable object is %d"), ((distance  < 0) ? (distance * -1.f) : distance));
		}
		else
		{
			//Search for UScannableComponent in actor
			UActorComponent* tempPointer = ScannedActors[iScanned]->GetComponentByClass(UScannableComponent::StaticClass());
			//Casting UActorComponent* to UscannableComponent*
			UScannableComponent *ScannablePointer = static_cast<UScannableComponent*>(tempPointer);

			//If UScannableComponent is found, mark component as scanned
			if (tempPointer != NULL && !ScannablePointer->IsScanned())
			{
				//get distance to owner of scannable component and log it
				//float distance = this->GetHorizontalDistanceTo(ScannedActors[iScanned]);
				//UE_LOG(LogClass, Log, TEXT("Distance to scannable object is %d"), ((distance < 0) ? (distance * -1.f) : distance));
			}
		}
	}

}

void UScanComponent::ChangeMode()
{
	int32 temp;
	ModesQueue.Dequeue(temp);
	ModesQueue.Enqueue(temp);

	int32 mode;

	ModesQueue.Peek(mode);

	UE_LOG(LogClass, Log, TEXT("You've changed the scanning mode to %i"), mode);
}

