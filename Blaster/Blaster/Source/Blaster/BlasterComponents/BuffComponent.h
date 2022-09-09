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
	UBuffComponent();

	//Functions to specify the default values for specific player stats when their respective buffs run out
	void SetInitialSpeeds(float base, float crouch);
	void SetInitialJumpVelocity(float velocity);

	//Avaliable buffs, all of them receive a buff amount and a time, wich can mean the duration of the buff or in case of the health and shield the time that it
	// will take for the buff to replenish all the health/shield picked up
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ShieldTime);
	void BuffSpeed(float BaseSpeed, float CrouchSpeed, float time);
	void BuffJump(float jumpVel, float time);

protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// Function that updates the character health with the healing rate left from previous healing pickups
	/// </summary>
	void HealRampUp(float DeltaTime);
	/// <summary>
	/// Function that updates the character shield with the healing rate left from previous shielding pickups
	/// </summary>
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

	/// <summary>
	/// Multicast that notifies the clients about the value of the speed stats that a character needs to set
	/// </summary>
	/// <param name="baseJump"> Value that needs to be set </param>
	UFUNCTION(NetMulticast, Reliable)		void MulticastSpeedBuff(float baseSpeed, float crouchSpeed);
	/// <summary>
	/// Funstion that resets the speed stat
	/// </summary>
	void ResetSpeed();
	FTimerHandle SpeedBuffTimer;

	/// <summary>
	/// Multicast that notifies the clients about the value of the jump stats that a character needs to set
	/// </summary>
	/// <param name="baseJump"> Value that needs to be set </param>
	UFUNCTION(NetMulticast, Reliable)		void MulticastJumpBuff(float baseJump);
	/// <summary>
	/// Function that resets the jump stat
	/// </summary>
	void ResetJump();
	FTimerHandle JumpBuffTimer;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};