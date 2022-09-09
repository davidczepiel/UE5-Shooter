// Fill out your copyright notice in the Description page of Project Settings.

#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{
	//Allows the tick function to be called
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//In case the player is healing or shielding the variables are updated over time
	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) return;

	//Healing amount that needs to be applied this tick
	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	//If we have already healed all the amount that the player picked up the healing proccess is stopped
	AmountToHeal -= HealThisFrame;
	if (AmountToHeal <= 0 || Character->GetHealth() >= Character->GetMaxHealth()) {
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bShielding || Character == nullptr || Character->IsElimmed()) return;

	//Shielding amount that needs to be applied this tick
	const float HealThisFrame = ShieldRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + HealThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	AmountToShield -= HealThisFrame;
	//If we have already shielded all the amount that the player picked up the shielding proccess is stopped
	if (AmountToShield <= 0 || Character->GetShield() >= Character->GetMaxShield()) {
		bShielding = false;
		AmountToShield = 0.f;
	}
}

//////////////////////////////////////////////////Setters/////////////////////////////////////////////
void UBuffComponent::SetInitialSpeeds(float base, float crouch)
{
	InitialBaseSpeed = base;
	InitialCrouchSpeed = crouch;
}

void UBuffComponent::SetInitialJumpVelocity(float base)
{
	InitialJumpVelocity = base;
}

//////////////////////////////////////////////////Buffs/////////////////////////////////////////////
void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void UBuffComponent::ReplenishShield(float ShieldAmount, float ShieldTime)
{
	bShielding = true;
	ShieldRate = ShieldAmount / ShieldTime;
	AmountToShield += ShieldAmount;
}

void UBuffComponent::BuffSpeed(float BaseSpeed, float CrouchSpeed, float time)
{
	if (Character == nullptr) return;

	//The timer to reset the speed is set, if the character is available then its speed is set to the amount specified by the parameter
	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &ThisClass::ResetSpeed, time);
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	}
	//RPC is called to notify all the players
	MulticastSpeedBuff(BaseSpeed, CrouchSpeed);
}

void UBuffComponent::BuffJump(float jumpbuff, float time)
{
	if (Character == nullptr) return;

	//The timer to reset the jump is set, if the character is available then its jump is set to the amount specified by the parameter
	Character->GetWorldTimerManager().SetTimer(JumpBuffTimer, this, &ThisClass::ResetJump, time);
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->JumpZVelocity = jumpbuff;
	}
	//RPC is called to notify all the players
	MulticastJumpBuff(jumpbuff);
}

void UBuffComponent::ResetSpeed()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	//If the character is available its speed stats are reset
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	}
	//RPC is called to notify all the players
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	//If the character is available its speed stats are reset
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;
	}
	//RPC is called to notify all the players
	MulticastJumpBuff(InitialJumpVelocity);
}

//This function is meant to be called to notify all the players in the session to set a specific characters speed stats
void UBuffComponent::MulticastSpeedBuff_Implementation(float baseSpeed, float crouchSpeed)
{
	//If there is an avaiable character its speed stats are set to the parameter values
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = baseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
	}
}

//This function is meant to be called to notify all the players in the session to set a specific characters jump stats
void UBuffComponent::MulticastJumpBuff_Implementation(float baseJump)
{
	//If there is an avaiable character its jump stats are set to the parameter values
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->JumpZVelocity = baseJump;
	}
}