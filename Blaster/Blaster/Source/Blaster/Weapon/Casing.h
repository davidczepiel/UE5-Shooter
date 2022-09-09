// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACasing();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// <summary>
	/// Funciton called whenever the casing hits anything
	/// </summary>
	/// <param name="HitComp"> Component that hit the casing </param>
	/// <param name="OtherActor"> Actor that hit the casing</param>
	/// <param name="OtherComponent"> Component that hit the casing </param>
	/// <param name="NormalImpulse"> Normal of the impact point</param>
	/// <param name="Hit"> Hit result from the collision </param>
	/// <returns></returns>
	UFUNCTION()	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)	UStaticMeshComponent* CasingMesh;
	UPROPERTY(EditAnywhere)		class USoundCue* ShellSound;
	UPROPERTY(EditAnywhere)		float ShellEjectionImpulse;

public:
};
