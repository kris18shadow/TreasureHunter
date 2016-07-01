// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ScanComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class TREASUREHUNTER_API UScanComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Scanning sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ScanningSphere;
	/** Modes queue */
	TQueue<int32> ModesQueue;

protected:
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
	/** Returns ScanningSphere subobject **/
	FORCEINLINE class USphereComponent* GetScanningSphere() const { return ScanningSphere; }

	// Sets default values for this component's properties
	UScanComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
