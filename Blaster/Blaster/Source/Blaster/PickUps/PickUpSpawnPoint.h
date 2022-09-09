// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class BLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	/// <summary>
	/// THis function is called whenever a pickup spawn point needs to create a new pickup
	/// </summary>
	void SpawnPickup();
	/// <summary>
	/// This funciton is called whenever the available pickup is picked and a timer to wait until a new one appears needs to start
	/// </summary>
	/// <param name="DestroyedActor"></param>
	/// <returns></returns>
	UFUNCTION()		void StartSpawnPickupTimer(AActor* DestroyedActor);
	/// <summary>
	/// THis function is called whenever a pickup was picked and the timer to create a new one just finished
	/// </summary>
	void SpawnPickupTimerFinished();

	UPROPERTY(EditAnywhere)		TArray<TSubclassOf<class APickUp>> PickupClasses;
	UPROPERTY()		APickUp* SpawnedPickup;

private:
	//Rate of spawning pick ups
	FTimerHandle SpawnPickupTimer;
	UPROPERTY(EditAnywhere)		float SpawnPickupTimeMin;
	UPROPERTY(EditAnywhere)		float SpawnPickupTimeMax;
public:
};