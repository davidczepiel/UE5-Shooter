// Fill out your copyright notice in the Description page of Project Settings.

#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{
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

	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
	if (AmountToHeal <= 0 || Character->GetHealth() >= Character->GetMaxHealth()) {
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bShielding || Character == nullptr || Character->IsElimmed()) return;

	const float HealThisFrame = ShieldRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + HealThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	AmountToShield -= HealThisFrame;
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

	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &ThisClass::ResetSpeed, time);
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	}
	MulticastSpeedBuff(BaseSpeed, CrouchSpeed);
}

void UBuffComponent::BuffJump(float jumpbuff, float time)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(JumpBuffTimer, this, &ThisClass::ResetJump, time);
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->JumpZVelocity = jumpbuff;
	}
	MulticastJumpBuff(jumpbuff);
}

void UBuffComponent::ResetSpeed()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	}
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;
	}
	MulticastJumpBuff(InitialJumpVelocity);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float baseSpeed, float crouchSpeed)
{
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = baseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
	}
}

void UBuffComponent::MulticastJumpBuff_Implementation(float baseJump)
{
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->JumpZVelocity = baseJump;
	}
}