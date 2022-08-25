// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 8000.f

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class ABlasterCharacter;
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Weapon related
	void FireButtonPressed(bool bPressed);
	void EquipWeapon(class AWeapon* WeaponToEquip);
	void Reload();
	UFUNCTION(BlueprintCallable)		void FinishReloading();
	void UpdateAmmo();

protected:
	virtual void BeginPlay() override;

	//Fire and aiming
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)			void ServerSetAiming(bool bIsAiming);
	UFUNCTION()							void OnRep_EquippedWeapon();
	UFUNCTION(Server, Reliable)			void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	//Crosshairs
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);

	//Reloading
	UFUNCTION(Server, Reliable)		void ServerReload();
	void HandleReload();
	int32 AmountToReload();

private:
	//Character
	class ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	//Combat
	UFUNCTION()					void OnRep_CombatState();
	UFUNCTION()					void OnRep_CarriedAmmo();
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)		ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)		class AWeapon* EquippedWeapon;
	UPROPERTY(Replicated)		bool bAiming;
	void InitCarriedAmmo();
	TMap<EWeaponType, int32> CarriedAmmoMap;
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)		int32 CarriedAmmo;
	UPROPERTY(EditAnywhere)								int32 StartingRifleAmmo = 30;
	UPROPERTY(EditAnywhere)								int32 StartingRocketAmmo = 0;
	UPROPERTY(EditAnywhere)								int32 StartingPistolAmmo = 20;

	//Automatic timer
	void FireTimerFinished();
	void StartFireTimer();
	void Fire();
	FTimerHandle FireTimer;
	bool bCanFire = true;
	bool bFireButtonPressed;
	bool CanFire();

	//Movement
	UPROPERTY(EditAnywhere)		float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)		float AimWalkSpeed;

	//HUD and Crosshairs
	float CrosshairVelFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FHUDPackage HUDPackage;
	FVector HitTarget;

	// Aiming and FOV
	float DefaultFov;
	UPROPERTY(EditAnywhere, Category = Combat)		float ZoomedFov = 30.f;
	UPROPERTY(EditAnywhere, Category = Combat)		float ZoomInterpSpeed = 20.f;
	float CurrentFov;
	void InterpFOV(float DeltaTime);

public:
};
