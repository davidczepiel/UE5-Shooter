// Fill out your copyright notice in the Description page of Project Settings.
#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//Checks if the thing this projectile has collided with is an player
	ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	//If so, that players controller is obtained and damage is applied
	if (OwnerCharacter) {
		ABlasterPlayerController* OwnerController = Cast< ABlasterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController) {
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind) {
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComp, OtherActor, OtherComponent, NormalImpulse, Hit);
				return;
			}
			ABlasterCharacter* character = Cast< ABlasterCharacter>(OtherActor);
			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled()) {
				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(character, TraceStart, InitialVelocity, OwnerController->GetServerTime() - OwnerController->SingleTripTime);
			}
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComponent, NormalImpulse, Hit);
}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif