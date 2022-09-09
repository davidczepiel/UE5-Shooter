// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoPickUp.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h"

void AAmmoPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//If a character collided with the hitbox, ammo is given to him and this pick up gets destroyed
	ABlasterCharacter* character = Cast<ABlasterCharacter>(OtherActor);
	if (character) {
		UCombatComponent* combat = character->GetCombat();
		if (combat) combat->PickUpAmmo(WeaponType, AmmoAmount);
	}
	Destroy();
}