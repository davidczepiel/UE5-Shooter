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
	virtual void Fire(const FVector& HitTarget);
	void AddAmmo(int32 Amount);
	void ShowPickupWidget(bool bShowWidget);
	void SetHUDWeaponAmmo();
	void Dropped();
	void SetWeaponState(EWeaponState NewState);

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

	USphereComponent* GetAreaShpere();

	bool bDestroyWeapon = false;

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

private:

	UFUNCTION()		void OnRep_WeaponState();
	UFUNCTION()		void OnRep_Ammo();
	void SpendRound();

	//Weapon assets
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")		class USphereComponent* AreaSphere;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")		class UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")			class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere)		TSubclassOf<class ACasing>			CasingClass;

	//Weapon data
	UPROPERTY(EditAnywhere)																				EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_WeaponState)		EWeaponState WeaponState;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_Ammo)				int32 CurrentAmmo = 30;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")												int32 MaxAmmo = 30;
	UPROPERTY()																							class ABlasterPlayerController* OwnerController;
	UPROPERTY()																							class ABlasterCharacter* OwnerCharacter;

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomFOV() const { return ZoomedFov; }
	FORCEINLINE bool HasAmmo() const { return CurrentAmmo > 0; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE int32 GetAmmo() const { return CurrentAmmo; }
	FORCEINLINE int32 GetMagCapacity() const { return MaxAmmo; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
};
