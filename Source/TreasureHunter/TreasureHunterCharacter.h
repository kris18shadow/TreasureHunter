// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "TreasureHunterCharacter.generated.h"

UCLASS(config=Game)
class ATreasureHunterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Scanning sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scanning", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ScanningSphere;

	/** Modes queue */
	TQueue<int32> ModesQueue;

public:
	ATreasureHunterCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Called when we press a key to scan any scannables in scan sphere*/
	UFUNCTION(BlueprintCallable, Category = "Scanning")
	void Scan();

	/** Scanning objects and setting fire to them*/
	UFUNCTION(BlueprintCallable, Category = "Scanning")
	void ScanMode0(TArray<AActor*> ScannedActors);

	/** Scanning objects and destroying them*/
	UFUNCTION(BlueprintCallable, Category = "Scanning")
	void ScanMode1(TArray<AActor*> ScannedActors);

	/** Lifting scannable objects(without marking them as scanned)*/
	UFUNCTION(BlueprintCallable, Category = "Scanning")
	void ScanMode2(TArray<AActor*> ScannedActors);

	/** Logging distance from scannable objects to player*/
	UFUNCTION(BlueprintCallable, Category = "Scanning")
	void ScanMode3(TArray<AActor*> ScannedActors);

	/** Called when we press a key to scan any scannables in scan sphere*/
	UFUNCTION(BlueprintCallable, Category = "Scanning")
	void ChangeMode();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns ScanningSphere subobject **/
	FORCEINLINE class USphereComponent* GetScanningSphere() const { return ScanningSphere; }
};

