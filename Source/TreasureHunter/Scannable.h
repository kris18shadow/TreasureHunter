// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Scannable.generated.h"

UCLASS()
class TREASUREHUNTER_API AScannable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScannable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Return the mesh for scannable */
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return ScannableMesh; }

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

private:
	/** Static mesh to represent scannable object */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scannable", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ScannableMesh;	
};
