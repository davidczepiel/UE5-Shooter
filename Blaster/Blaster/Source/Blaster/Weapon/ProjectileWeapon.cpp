// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	//If this weapon is not at the server, fire is prevented
	if (!HasAuthority()) return;

	//The shooter is obtained and the socket from were the bullet must start is fetched
	APawn* instigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName(TEXT("MuzzleFlash")));

	//If the spawn is obtained, a bullet is spawned
	if (MuzzleFlashSocket) {
		//Direction is calculated for the bullet
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		//If a projectile is provided and the shooter has been obtained succesfully the bullet is fired
		if (ProjectileClass && instigatorPawn) {
			FActorSpawnParameters spawnparameters;
			spawnparameters.Owner = GetOwner();
			spawnparameters.Instigator = instigatorPawn;

			UWorld* world = GetWorld();
			if (world) {
				world->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, spawnparameters);
			}
		}
	}
}