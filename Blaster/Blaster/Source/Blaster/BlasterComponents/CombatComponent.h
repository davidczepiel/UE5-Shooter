// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 8000.f

/**
* Class that is meant to manage everything related to combat, having a weapon, managing the ammo...
*/

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class ABlasterCharacter;
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/// <summary>
	/// This function is meant to take a weapon as a parameter and equip it so that the character can use it
	/// </summary>
	/// <param name="WeaponToEquip"> Weapon that the character takes as its next equipped weapon</param>
	void EquipWeapon(class AWeapon* WeaponToEquip);

	/// <summary>
	/// Function that is going to be called to notify that the player is pressing or stopped pressing the fire button
	/// </summary>
	/// <param name="bPressed"> Bool that specifies if the fire button was pressed or released </param>
	void FireButtonPressed(bool bPressed);

	/// <summary>
	/// Function that adds ammo to the characters ammo stocks from a specific type of weapon
	/// </summary>
	/// <param name="type"> Weapon type that uses this specific ammo </param>
	/// <param name="amount"> Amount of bullets to add </param>
	void PickUpAmmo(EWeaponType type, int32 amount);

	/// <summary>
	/// Function that takes care of updating the HUD ammo amount displayed to the player and to update the ammo stocks left after reloading or picking ammo
	/// </summary>
	void UpdateAmmo();
	/// <summary>
	/// Function that takes care of updating the HUD ammo amount displayed to the player for the magacine ammo left on the carried weapon each time the player reloads or shoots a bullet
	/// </summary>
	void UpdateCarriedAmmo();

	/// <summary>
	/// Function that is called from other components that takes care of notifying the server that this character is currently reloading on a client machine and that calls other functions
	/// to take care of the animation part of reloading
	/// </summary>
	void Reload();
	/// <summary>
	/// Function that is going to be called from blueprints (called when the reload animation finishes) ant takes care of updating the ammo amounts and letting the player continue the shooting
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)		void FinishReloading();

	bool bLocallyReloading = false;

protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// Function that is going to be called from another component and will take care of setting the character speed to the aiming speed. Also notifyes the server that the character is currently aiming
	/// </summary>
	/// <param name="bIsAiming"></param>
	void SetAiming(bool bIsAiming);
	/// <summary>
	/// RPC that is going to be called from clients to notify the server that a specific client is currently aiming
	/// </summary>
	/// <param name="bIsAiming"> Bool that specifies if the character is aiming or not </param>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)			void ServerSetAiming(bool bIsAiming);
	UFUNCTION()							void OnRep_EquippedWeapon();

	/// <summary>
	/// RPC that is going to be called form clients to notify the server that a client is currently trying to fire
	/// This function only calls another RPC to notify the rest of the clients
	/// </summary>
	/// <param name="TraceHitTarget"> Point that a client is trying to fire at </param>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)			void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	/// <summary>
	/// RPC that is going to be called from the server to notify all the players that a specific character is currently shooting
	/// </summary>
	/// <param name="TraceHitTarget"> Point that a client is trying to fire at </param>
	/// <returns></returns>
	UFUNCTION(NetMulticast, Reliable)	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	/// <summary>
	///Function that calculates the middle point of the screen and projects it to the world to get the exact point that the next bullet shoulg follow when the player shoots
	/// </summary>
	/// <param name="TraceHitResult"> Out parameter that is going to contain the result of tracing the shot </param>
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	/// <summary>
	/// Function that is going to take care of drawwing the crosshairs at the correct position on the screen, take care of player movement, shooting...
	/// </summary>
	/// <param name="DeltaTime"> Delta time to calculate the crosshair spread </param>
	void SetHUDCrosshairs(float DeltaTime);

	/// <summary>
	/// RPC that is going to be called from the clients to notify the server that a character wants to reload, this RPC will update the combat state to reloading and start the reloading animation
	/// </summary>
	UFUNCTION(Server, Reliable)		void ServerReload();
	/// <summary>
	/// Function that plays the reload animation on the character
	/// </summary>
	void HandleReload();
	/// <summary>
	/// FUnction that calculates the amount of space left in the magacine and from it takes the correct amount of bullets left on the player stocks
	/// </summary>
	/// <returns> Amount of bullets that the player can reload </returns>
	int32 AmountToReload();

private:
	//Character
	class ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	//Combat
	UFUNCTION()												void OnRep_CombatState();
	UFUNCTION()												void OnRep_CarriedAmmo();
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)			ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)		class AWeapon* EquippedWeapon;

	//Aiming
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)		bool bAiming = false;
	bool bAimButtonPressed = false;
	UFUNCTION()		void OnRep_Aiming();

	//Ammo variables for all the available weapons in the game
	void InitCarriedAmmo();
	TMap<EWeaponType, int32> CarriedAmmoMap;
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)		int32 CarriedAmmo;
	UPROPERTY(EditAnywhere)								int32 StartingRifleAmmo = 30;
	UPROPERTY(EditAnywhere)								int32 StartingRocketAmmo = 0;
	UPROPERTY(EditAnywhere)								int32 StartingPistolAmmo = 20;

	//Automatic timer

	/// <summary>
	/// Function that is going to be called from other components and will take care of firing if possible, if possible, it will notify the server to let the other clients know that this charcter is firing
	/// </summary>
	void Fire();
	/// <summary>
	/// Function that sees if all the requirements to fire are satisfied (to have ammo, to not be reloading, to have a weapon...)
	/// </summary>
	/// <returns> Value that specifies if the character can fire or not </returns>
	bool CanFire();
	/// <summary>
	/// Function that sets a timer for the weapon to add a delay betwean each bullet shot (doesnt matter if the weapon is automatic or not)
	/// </summary>
	void StartFireTimer();
	/// <summary>
	/// Function called when the delay betwean bullets timer finished, this lets the player continue firing, if the fire button is still pressed and the weapon is automatic, the firing continues
	/// </summary>
	void FireTimerFinished();
	/// <summary>
	/// Function that fires the weapon but only on the players machine, this is to smooth out lag effects
	/// </summary>
	/// <param name="TraceHitTarget"></param>
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	FTimerHandle FireTimer;
	bool bCanFire = true;
	bool bFireButtonPressed;

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
	/// <summary>
	/// Function that interps the current FOV to create a smooth transition from aiming to not aiming
	/// </summary>
	void InterpFOV(float DeltaTime);

public:
};
