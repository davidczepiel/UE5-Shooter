// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "AmmoPickUp.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API AAmmoPickUp : public APickUp
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
	) override;

private:
	UPROPERTY(EditAnywhere)		int32 AmmoAmount = 30;
	UPROPERTY(EditAnywhere)		EWeaponType WeaponType;
};
