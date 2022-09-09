// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"

#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera()) {
			DefaultFov = Character->GetFollowCamera()->FieldOfView;
			CurrentFov = DefaultFov;
		}

		//Only on the server because the ammo value will replicate
		if (Character->HasAuthority())			InitCarriedAmmo();
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled()) {
		//Trace the point where the next bullet will go if shooted
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		//Crosshair placement and FOV when aiming/not aiming
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::InitCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
}

//////////////////////////////////CROSSHAIRS//////////////////////////////////////////////

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr)return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller) {
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD) {
			//If we have a valid HUD, and the character has a weapon equipped then the crosshair is rendered
			if (EquippedWeapon) {
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}

			//Calculate the crosshair spread
			//[0,600] -> [0,1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelMultiplierRange(0.f, 1.f);
			FVector Vel = Character->GetVelocity();
			Vel.Z = 0;
			CrosshairVelFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelMultiplierRange, Vel.Size());

			//The crosshairs are moved if the character is in the air
			if (Character->GetCharacterMovement()->IsFalling()) 	CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			else													CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);

			//If the aiming factor is a negative number that will decrease the space betwean crosshair if aiming
			if (bAiming) 	CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			else 			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);

			//The firing factor is reduced each tick, it will be increased for every bullet the player shoots
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			//Space is set and the HUD is updated
			HUDPackage.CrosshairSpread = CrosshairVelFactor + 0.5f + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)	GEngine->GameViewport->GetViewportSize(ViewportSize);

	//Transformation from screen point to world position
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	//If the projection was succesful
	if (bScreenToWorld)
	{
		//Limits of the trace performed
		FVector Start = CrosshairWorldPosition;
		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		//If the trace collides with whatever that is not world bounds (visibility channel) then an impact point will be provided
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		//If there was no impact point the direction the bullet should go to is the trace end
		if (!TraceHitResult.bBlockingHit) {
			TraceHitResult.ImpactPoint = End;
		}

		//The Crosshair is pointing at a character
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())		HUDPackage.CrosshairsColor = FLinearColor::Red;
		else																											HUDPackage.CrosshairsColor = FLinearColor::White;
	}
}

//////////////////////////////AIMING/////////////////////////////////////////////////////

void UCombatComponent::SetAiming(bool bIsAiming)
{
	//The character starts aiming and the server is notified
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);

	//If the character is avaliable the speed is updated
	if (Character)							Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	if (Character->IsLocallyControlled())	bAimButtonPressed = bIsAiming;
}

void UCombatComponent::OnRep_Aiming()
{
	//If the character is locally controlled, this operation is done to prevent strange behaviour when just tapping the aim button
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	//The server is notified and the client that started aiming is set to aim
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	//FOV is interpolated depending on if the player is currently aiming or not
	if (bAiming) 	CurrentFov = FMath::FInterpTo(CurrentFov, EquippedWeapon->GetZoomFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	else			CurrentFov = FMath::FInterpTo(CurrentFov, DefaultFov, DeltaTime, ZoomInterpSpeed);

	if (Character && Character->GetFollowCamera()) {
		Character->GetFollowCamera()->SetFieldOfView(CurrentFov);
	}
}

///////////////////////////////////////FIRE////////////////////////////////////////

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	if (bLocallyReloading) return false;
	return EquippedWeapon->HasAmmo() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::Fire()
{
	if (CanFire()) {
		//Server is notified and a local shot is done
		ServerFire(HitTarget);
		if (!Character->HasAuthority())	LocalFire(HitTarget);
		StartFireTimer();

		//Crosshair factor is updated to increase its spread
		if (EquippedWeapon) {
			CrosshairShootingFactor = 1.f;
		}
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed) {
		Fire();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	//If the RPC comes back to the original client that started this notification this function ends here, because this local client has already done the local fire when he pressed the fire button
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;

	LocalFire(TraceHitTarget);
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	//This function is called only on the local machine
	if (EquippedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied) {
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;

	bCanFire = true;
	//If the player is still pressing the fire button and the weapon is automatic the weapon continues firing
	if (bFireButtonPressed && EquippedWeapon->bAutomatic) 		Fire();
	//If the weapon has no ammo the character tries to reload
	if (!EquippedWeapon->HasAmmo())								Reload();
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	bCanFire = false;
	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);
}

/////////////////////////////RELOAD////////////////////////////////////////

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading && !bLocallyReloading) {
		//Server is notified and the animation is played to smooth out the lag effects
		ServerReload();
		HandleReload();
		bLocallyReloading = true;
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr)return;

	CombatState = ECombatState::ECS_Reloading;
	//Only if the character is not locally controlled because otherwise it would have already started the reloading animation
	if (!Character->IsLocallyControlled()) HandleReload();
}

void UCombatComponent::HandleReload()
{
	if (Character) {
		Character->PlayReloadMontage();
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType())) {
		int32 Carried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, Carried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr)return;
	bLocallyReloading = false;

	//Only the server because all the variables changed will replicate to clients
	if (Character->HasAuthority()) {
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmo();
		UpdateCarriedAmmo();
	}
	if (bFireButtonPressed) {
		Fire();
	}
}

void UCombatComponent::UpdateAmmo()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	//Weapon ammo is udpated
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType())) {
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	//HUD for magacine ammo is updated
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller) 		Controller->SetHUDWeaponAmmo(CarriedAmmo);

	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	//Weapon magacine ammo is updated on the HUD
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)		Controller->SetHUDCarriedAmmo(CarriedAmmo);
}

void UCombatComponent::PickUpAmmo(EWeaponType type, int32 amount)
{
	if (CarriedAmmoMap.Contains(type)) {
		CarriedAmmoMap[type] += amount;

		//If we are carring a weapon type the same as the ammo picked up type, HUD must be updated
		if (EquippedWeapon->GetWeaponType() == type) {
			CarriedAmmo += amount;
			UpdateCarriedAmmo();
		}
	}

	//If we got an empty weapon and we just got ammo for that specific type of weapon we reload it automatically
	if (EquippedWeapon && !EquippedWeapon->HasAmmo() && EquippedWeapon->GetWeaponType() == type) Reload();
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller) {
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

/////////////////////////////////////////////////////COMBAT///////////////////////////////////

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	//Drop the weapon in hands if exists
	if (EquippedWeapon) 		EquippedWeapon->Dropped();

	//The new weapon is set to the correct location on the character hands
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket) 		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());

	EquippedWeapon->SetOwner(Character);
	//THe HUD is updated with this weapons magacine ammo left
	EquippedWeapon->SetHUDWeaponAmmo();

	//In case we changed weapon typed the carried ammo must be updated on the HUD
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType())) 		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller) 		Controller->SetHUDCarriedAmmo(CarriedAmmo);

	//If the weapon picked is empty we try to reload it
	if (!EquippedWeapon->HasAmmo()) 	Reload();

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	if (EquippedWeapon->EquipSound) UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState) {
	case ECombatState::ECS_Reloading:
		//iF the reloading animation is till not playing we play it
		if (Character && !Character->IsLocallyControlled())HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		//If the player is already pressing the fire button the weapon is fired
		if (bFireButtonPressed) {
			Fire();
		}
		break;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	//Weapon is updated for the clients
	if (EquippedWeapon && Character)
	{
		//It is attached to the correct position relative to the characters hands
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket) 			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		if (EquippedWeapon->EquipSound) UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
	}
}