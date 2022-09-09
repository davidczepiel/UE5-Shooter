// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	/// <summary>
	/// Function that is called from another component which traces a line from the weapon to the hit target location looking for any kind of actor to hit
	/// </summary>
	virtual void Fire(const FVector& HitTarget) override;
protected:

	/// <summary>
	/// Function that is colled when firing to trace a line and try to see if the shot actually connectec with another actor
	/// </summary>
	/// <param name="TraceStart"> Start of the line trace </param>
	/// <param name="HitTarget"> End of the line trace </param>
	/// <param name="OutHit"> Result of the trace </param>
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	//Effects for shooting
	UPROPERTY(EditAnywhere)		class UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)		USoundCue* HitSound;
private:
	//Effects for the bullet
	UPROPERTY(EditAnywhere)		UParticleSystem* BeamParticles;
	UPROPERTY(EditAnywhere)		UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)		USoundCue* FireSound;
};