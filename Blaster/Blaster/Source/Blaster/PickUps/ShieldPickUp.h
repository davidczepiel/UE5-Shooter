// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "ShieldPickup.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API AShieldPickup : public APickUp
{
	GENERATED_BODY()
protected:
	/// <summary>
	/// Callback that gets called whenever the hitbox collides with a player and as a consecuence it is picked by him
	/// </summary>
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
private:
	UPROPERTY(EditAnywhere)		float ShieldReplenishAmount = 100.f;
	UPROPERTY(EditAnywhere)		float ShieldReplenishTime = 5.f;
};