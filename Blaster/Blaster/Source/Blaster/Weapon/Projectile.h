// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)		float Damage = 20.f;
private:
	//Components necessary to offer a realistic behaviour for a bullet
	UPROPERTY(EditAnywhere)		class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere)		class UParticleSystem* Tracer;
	UPROPERTY(EditAnywhere)		class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere)		UParticleSystem* ImpactParticles;
	class UParticleSystemComponent* TracerComponent;

public:
};
