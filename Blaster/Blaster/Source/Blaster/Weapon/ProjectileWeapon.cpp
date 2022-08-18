// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;
	APawn* instigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName(TEXT("MuzzleFlash")));
	if (MuzzleFlashSocket) {
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

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