// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "JumpPickUp.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API AJumpPickUp : public APickUp
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	UPROPERTY(EditAnywhere)
		float JumpBuff = 4000.f;

	UPROPERTY(EditAnywhere)
		float JumpBuffTime = 15.f;
};
