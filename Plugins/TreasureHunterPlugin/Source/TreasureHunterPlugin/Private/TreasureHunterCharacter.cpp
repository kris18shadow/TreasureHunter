// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TreasureHunter.h"
#include "TreasureHunterCharacter.h"
#include "Scannable.h"
#include "ScannableComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATreasureHunterCharacter

ATreasureHunterCharacter::ATreasureHunterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create a scanning sphere
	ScanningSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ScanningSphere"));
	ScanningSphere->AttachTo(RootComponent);
	ScanningSphere->SetSphereRadius(300.f);

	//Create 4 modes
	for (int32 i = 0; i < 4; i++)
	{
		ModesQueue.Enqueue(i);
	}

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATreasureHunterCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Scan", IE_Pressed, this, &ATreasureHunterCharacter::Scan);
	InputComponent->BindAction("ChangeMode", IE_Pressed, this, &ATreasureHunterCharacter::ChangeMode);
	InputComponent->BindAxis("MoveForward", this, &ATreasureHunterCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATreasureHunterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ATreasureHunterCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ATreasureHunterCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &ATreasureHunterCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &ATreasureHunterCharacter::TouchStopped);
}


void ATreasureHunterCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ATreasureHunterCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ATreasureHunterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATreasureHunterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATreasureHunterCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATreasureHunterCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void ATreasureHunterCharacter::Scan()
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

void ATreasureHunterCharacter::ScanMode0(TArray<AActor*> ScannedActors)
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

void ATreasureHunterCharacter::ScanMode1(TArray<AActor*> ScannedActors)
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

void ATreasureHunterCharacter::ScanMode2(TArray<AActor*> ScannedActors)
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

void ATreasureHunterCharacter::ScanMode3(TArray<AActor*> ScannedActors)
{
	//If actor is overlapping with scanning sphere, mark actor as scanned
	for (int32 iScanned = 0; iScanned < ScannedActors.Num(); ++iScanned)
	{
		AScannable* const TestScan = Cast<AScannable>(ScannedActors[iScanned]);
		if (TestScan && !TestScan->IsPendingKill() && !TestScan->IsScanned())
		{
			//get distance to scannable object and log it
			float distance = this->GetHorizontalDistanceTo(TestScan);
			UE_LOG(LogClass, Log, TEXT("Distance to scannable object is %d"), ((distance  < 0) ? (distance * -1.f) : distance));
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
				float distance = this->GetHorizontalDistanceTo(ScannedActors[iScanned]);
				UE_LOG(LogClass, Log, TEXT("Distance to scannable object is %d"), ((distance < 0) ? (distance * -1.f) : distance));
			}
		}
	}

}

void ATreasureHunterCharacter::ChangeMode()
{
	int32 temp;
	ModesQueue.Dequeue(temp);
	ModesQueue.Enqueue(temp);

	int32 mode;

	ModesQueue.Peek(mode);

	UE_LOG(LogClass, Log, TEXT("You've changed the scanning mode to %i"), mode);
}