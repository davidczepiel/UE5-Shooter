// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();
	friend class ABlasterCharacter;

	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ShieldTime);
	void BuffSpeed(float BaseSpeed, float CrouchSpeed, float time);
	void BuffJump(float jumpVel, float time);
	void SetInitialSpeeds(float base, float crouch);
	void SetInitialJumpVelocity(float velocity);

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

private:
	UPROPERTY()
		class ABlasterCharacter* Character;

	bool bShielding = false;
	float ShieldRate = 0.f;
	float AmountToShield = 0.f;

	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	//Buff Timers
	FTimerHandle SpeedBuffTimer;
	void ResetSpeed();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpeedBuff(float baseSpeed, float crouchSpeed);

	FTimerHandle JumpBuffTimer;
	float InitialJumpVelocity;
	void ResetJump();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastJumpBuff(float baseJump);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};