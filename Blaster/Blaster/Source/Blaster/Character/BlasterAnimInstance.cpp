// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterTypes/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (BlasterCharacter == nullptr) BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	if (BlasterCharacter == nullptr) return;

	//Checks for major variables related to body poses, rotation and weapon
	bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();

	//Speed
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	//Character upper body rotation
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	TurningInPlace = BlasterCharacter->GetTurningInPlace();
	bElim = BlasterCharacter->IsElimmed();

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	//If a weapon is equipped the players upper body is handled separately
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() != nullptr && BlasterCharacter->GetMesh() != nullptr) {
		//Hand position are obtained to determine where should the equipped weapon be
		FVector OutPosition;
		FRotator OutRotation;
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		//If this character is a proxy, the aimimng acuracy(skeleton) must be higher
		if (BlasterCharacter->IsLocallyControlled()) {
			bLocallyControlled = true;
			FTransform RightHandTr = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			FRotator lookatrotation = UKismetMathLibrary::FindLookAtRotation(RightHandTr.GetLocation(), RightHandTr.GetLocation() + (RightHandTr.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, lookatrotation, DeltaTime, 30.f);
		}

		//Hand transformations must only occur if the player is not reloading, cause if not, the animations look strange
		bUSeFabrik = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
		if (BlasterCharacter->IsLocallyControlled()) {
			bUSeFabrik = !BlasterCharacter->IsLocallyReloading();
		}
		bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
		bTransformRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();

		//Debug lines from weapon to screen center
		//FTransform MuzzleTiptransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		//FVector MuzzleX(FRotationMatrix(MuzzleTiptransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		//DrawDebugLine(GetWorld(), MuzzleTiptransform.GetLocation(), MuzzleTiptransform.GetLocation() + MuzzleX * 1000, FColor::Red);
		//DrawDebugLine(GetWorld(), MuzzleTiptransform.GetLocation(), BlasterCharacter->GetHitTarget(), FColor::Green);
	}
}