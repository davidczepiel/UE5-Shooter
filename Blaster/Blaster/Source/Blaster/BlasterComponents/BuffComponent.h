// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

/**
* Component attached to the player that is meant to manage everything related to player buffs (strength of the effects, duration...)
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class ABlasterCharacter;
	//Init and config
	UBuffComponent();
	void SetInitialSpeeds(float base, float crouch);
	void SetInitialJumpVelocity(float velocity);

	//Avaliable buffs
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ShieldTime);
	void BuffSpeed(float BaseSpeed, float CrouchSpeed, float time);
	void BuffJump(float jumpVel, float time);

protected:
	virtual void BeginPlay() override;

	//Functions that control the healing over time
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

private:
	//Basic character info
	UPROPERTY()		class ABlasterCharacter* Character;
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	float InitialJumpVelocity;

	//Shield variables
	bool bShielding = false;
	float ShieldRate = 0.f;
	float AmountToShield = 0.f;
	//Healing variables
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	//Speed buff
	UFUNCTION(NetMulticast, Reliable)		void MulticastSpeedBuff(float baseSpeed, float crouchSpeed);
	void ResetSpeed();
	FTimerHandle SpeedBuffTimer;

	//Jump buff
	UFUNCTION(NetMulticast, Reliable)		void MulticastJumpBuff(float baseJump);
	void ResetJump();
	FTimerHandle JumpBuffTimer;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};