// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

UCLASS()
class BLASTER_API APickUp : public AActor
{
	GENERATED_BODY()

public:
	APickUp();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// Callback that gets called whenever the hitbox collides with a player and as a consecuence it is picked by him
	/// </summary>
	UFUNCTION()		virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)		float BaseTurnRate = 45.f;
private:
	//Hitbox to determine if the player is able to pick up this item or not
	UPROPERTY(EditAnywhere)		class USphereComponent* OverlapSphere;

	//Visual effects
	UPROPERTY(EditAnywhere)		UStaticMeshComponent* PickupMesh;
	UPROPERTY(VisibleAnywhere)	class UNiagaraComponent* PickupEffectComponent;
	UPROPERTY(EditAnywhere)		class UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)		class USoundCue* PickupSound;

	FTimerHandle BindOverlapTimer;
	float BindOverlapTime = 0.25f;
	void BindOverlapTimerFinished();
public:
};
