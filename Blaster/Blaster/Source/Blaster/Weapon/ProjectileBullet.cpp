// Fill out your copyright notice in the Description page of Project Settings.
#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//Checks if the thing this projectile has collided with is an player
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	//If so, that players controller is obtained and damage is applied
	if (OwnerCharacter) {
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController) 	UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
	}

	//Last because this method destroys actor
	Super::OnHit(HitComp, OtherActor, OtherComponent, NormalImpulse, Hit);
}