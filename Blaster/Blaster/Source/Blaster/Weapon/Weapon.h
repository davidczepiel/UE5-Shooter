// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Components/SphereComponent.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equiped UMETA(DisplayName = "Equiped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "Default Max")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;

	/// <summary>
	/// This function is called from another component and is meant to fire a bullet in a given direction
	/// </summary>
	/// <param name="HitTarget"> Direction where the bullet is going to be shot to </param>
	virtual void Fire(const FVector& HitTarget);
	/// <summary>
	/// Function called from another component that is supposed to add ammo to the wepaons magacine
	/// </summary>
	/// <param name="Amount"> Amount of ammo to add to the magacine</param>
	void AddAmmo(int32 Amount);
	/// <summary>
	/// Function called from another component that is suposed to show/hide the widget that shows the user how to equip the weapon
	/// </summary>
	/// <param name="bShowWidget"></param>
	void ShowPickupWidget(bool bShowWidget);
	/// <summary>
	/// Function that is supposed to notify another component to show the correct amount of ammo left on the weapon
	/// </summary>
	void SetHUDWeaponAmmo();
	/// <summary>
	/// Function that takes care of clearing all the weapon variables when it is dropped on the flor
	/// </summary>
	void Dropped();
	/// <summary>
	/// Sets the weapon state to a given one
	/// </summary>
	/// <param name="NewState"></param>
	void SetWeaponState(EWeaponState NewState);

	/// <summary>
	/// Function called when the weapons state changes to equipped
	/// </summary>
	void OnEquipped();
	/// <summary>
	/// Function called when the weapons state changes to dropped
	/// </summary>
	void OnDropped();

	//Crosshair textures
	UPROPERTY(EditAnywhere, Category = Crosshairs)		class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)		UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)		UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)		UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)		UTexture2D* CrosshairsBottom;

	//Zoom FOV while aiming
	UPROPERTY(EditAnywhere)		float ZoomedFov = 30.f;
	UPROPERTY(EditAnywhere)		float ZoomInterpSpeed = 20.f;

	//Automatic fire
	UPROPERTY(EditAnywhere, Category = Combat)		float FireDelay = 0.15f;
	UPROPERTY(EditAnywhere, Category = Combat)		bool bAutomatic = true;

	//Interaction sound
	UPROPERTY(EditAnywhere)		class USoundCue* EquipSound;

	//Hitbox to pick the weapon
	USphereComponent* GetAreaShpere();

	UPROPERTY(EditAnywhere)		float HeadShotDamage = 40.f;
	bool bDestroyWeapon = false;

	void EnableCustomDepth(bool enable);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()		virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()		virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UPROPERTY(EditAnywhere)					float Damage = 20.f;
	UPROPERTY(Replicated, EditAnywhere)		bool bUseServerSideRewind = false;

	UPROPERTY()																							class ABlasterPlayerController* OwnerController;
	UPROPERTY()																							class ABlasterCharacter* OwnerCharacter;

	/// <summary>
	/// Function that is called from another component that sets the weapon to use server side rewind in case of high lag
	/// </summary>
	/// <param name="bPingTooHigh"> Bool that says if the ping is too high and needs server side rewind or not</param>
	UFUNCTION()		void OnPingTooHigh(bool bPingTooHigh);
private:

	UFUNCTION()		void OnRep_WeaponState();
	/// <summary>
	/// Funciton that takes one bullet from the magacine, updates the HUD and notifies the server
	/// </summary>
	void SpendRound();

	/// <summary>
	/// RPC that is meant for the clients that notifies them that the server has processed their previous bullet spent
	/// </summary>
	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmo(int32 ServerAmmo);
	/// <summary>
	/// RPC for the clients that specifies the ammo amount that needs to be added to the weapon
	/// </summary>
	UFUNCTION(Client, Reliable)
		void ClientAddAmmo(int32 AmmoToAdd);

	//Number of unprocessed serer requests for Ammo (incremented in spend round/ decremented in clientAddAmmo)
	int32 Sequence = 0;

	//Weapon assets
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")		class USphereComponent* AreaSphere;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")		class UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")			class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere)		TSubclassOf<class ACasing>			CasingClass;

	//Weapon data
	UPROPERTY(EditAnywhere)																				EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_WeaponState)		EWeaponState WeaponState;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties"/*, ReplicatedUsing = OnRep_Ammo*/)				int32 CurrentAmmo = 30;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")												int32 MaxAmmo = 30;

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomFOV() const { return ZoomedFov; }
	FORCEINLINE bool HasAmmo() const { return CurrentAmmo > 0; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE int32 GetAmmo() const { return CurrentAmmo; }
	FORCEINLINE int32 GetMagCapacity() const { return MaxAmmo; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
};
