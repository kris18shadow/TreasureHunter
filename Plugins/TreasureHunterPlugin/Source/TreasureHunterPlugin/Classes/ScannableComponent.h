// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ScannableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class TREASUREHUNTER_API UScannableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UScannableComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;


	/** Returns if the object is Scanned*/
	UFUNCTION(BlueprintPure, Category = "Scannable")
		bool IsScanned();

	/** Sets new scanned state*/
	UFUNCTION(BlueprintCallable, Category = "Scannable")
		void SetScanned(bool newScannableState);

	UFUNCTION(BlueprintNativeEvent)
		void WasScanned();

	virtual void WasScanned_Implementation();
protected:
	/** True if is scanned, false if it is not scanned*/
	bool bIsScanned;
	
};
