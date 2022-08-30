// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Animation methods
	void RotateInPlace(float DeltaTime);
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayHitReactMontage();
	void SetTeamColor(ETeam Team);

	//Movement and death
	virtual void OnRep_ReplicatedMovement() override;
	virtual void Destroyed() override;
	UPROPERTY(Replicated)		bool bDisableGameplay = false;
	void UpdateHUDHealth();
	void UpdateHUDShield();

	//Player was eliminated
	UFUNCTION(NetMulticast, Reliable)		void MulticastElim(bool bPlayerLeftGame);
	void Elim(bool bPlayerLeftGame);

	//Weapon related
	void FireButtonPressed();
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	void TurnInplace(float DeltaTime);
	ECombatState GetCombatState();
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	UPROPERTY()		class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(EditAnywhere)		class UNiagaraSystem* CrownSystem;
	UPROPERTY()					class UNiagaraComponent* CrownComponent;

	void SpawnDefaultWeapon();

	//Hitboxes for server-side rewind
	UPROPERTY(EditAnywhere)		class UBoxComponent* head;

	UPROPERTY(EditAnywhere)		UBoxComponent* pelvis;
	UPROPERTY(EditAnywhere)		UBoxComponent* spine_02;
	UPROPERTY(EditAnywhere)		UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)		UBoxComponent* upperarm_l;
	UPROPERTY(EditAnywhere)		UBoxComponent* upperarm_r;
	UPROPERTY(EditAnywhere)		UBoxComponent* lowerarm_l;
	UPROPERTY(EditAnywhere)		UBoxComponent* lowerarm_r;
	UPROPERTY(EditAnywhere)		UBoxComponent* hand_l;
	UPROPERTY(EditAnywhere)		UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)		UBoxComponent* backpack;
	UPROPERTY(EditAnywhere)		UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)		UBoxComponent* thigh_l;
	UPROPERTY(EditAnywhere)		UBoxComponent* thigh_r;
	UPROPERTY(EditAnywhere)		UBoxComponent* calf_r;
	UPROPERTY(EditAnywhere)		UBoxComponent* calf_l;
	UPROPERTY(EditAnywhere)		UBoxComponent* foot_l;
	UPROPERTY(EditAnywhere)		UBoxComponent* foot_r;

	UPROPERTY()					TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	UFUNCTION(Server, Reliable)
		void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastLostTheLead();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PollInit();

	//Player Actions
	virtual void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	virtual void Jump() override;
	UFUNCTION()		void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DType, AController* InstigatorController, AActor* DamageCauser);

	//Buttons pressed
	void EquipButtonPress();
	void CrouchButtonPress();
	void ReloadButtonPress();
	void AimButtonPress();
	void AimButtonReleased();
	void FireButtonReleased();

	//Character rotation
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();

	void UpdateHUDAmmo();

private:
	//Camera
	UPROPERTY(VisibleAnywhere, Category = Camera)											class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)											class UCameraComponent* FollowCamera;
	void HideCameraIfCharacterClose();
	UPROPERTY(EditAnywhere)		float CameraThreshHold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))		class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)									class AWeapon* OverlappingWeapon;
	UFUNCTION()		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))		class UCombatComponent* Combat;
	UPROPERTY(VisibleAnywhere)		class UBuffComponent* Buff;
	UPROPERTY(VisibleAnywhere)		class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)		void ServerEquipButtonPress();

	float CalculateSpeed();

	//Animations
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;
	UPROPERTY(EditAnywhere, Category = Combat)		class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = Combat)		class UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category = Combat)		class UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = Combat)		class UAnimMontage* ElimMontage;
	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;

	//Health
	UFUNCTION()		void OnRep_Health(float lastHealth);
	UPROPERTY(EditAnywhere, Category = "Player Stats")											float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "Player Stats")		float CurrentHealth = 100.f;

	UFUNCTION()		void OnRep_Shield(float lastShield);
	UPROPERTY(EditAnywhere, Category = "Player Stats")											float MaxShield = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")		float CurrentShield = 25.f;

	bool bLeftGame = false;

	//Death
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)		float ElimDelay = 3.f;
	FTimerHandle ElimTimer;
	bool bElim = false;

	UPROPERTY()	class ABlasterPlayerController* BlasterPlayerController;

	//DisolveEffect on death
	void StartDissolve();
	UFUNCTION()		void UpdateDissolveMaterial(float DissolveValue);
	UPROPERTY(VisibleAnywhere)		UTimelineComponent* DissolveTimeline;
	UPROPERTY(EditAnywhere)		UCurveFloat* DissolveCurve;
	FOnTimelineFloat DissolveTrack;
	UPROPERTY(VisibleAnywhere, Category = Elim)		UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* DissolveMaterialInstance;

	/**
* Team colors
*/

	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* RedDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* RedMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* BlueDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* OriginalMaterial;

	//DefaultWeapon
	UPROPERTY(EditAnywhere)
		TSubclassOf<AWeapon> DefaultWeapon;

	UPROPERTY()
		class ABlasterGameMode* BlasterGameMode;

public:
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE float GetHealth() const { return CurrentHealth; }
	FORCEINLINE void SetHealth(float amount) { CurrentHealth = amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return CurrentShield; }
	FORCEINLINE void SetShield(float amount) { CurrentShield = amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE bool IsElimmed() const { return bElim; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	bool IsLocallyReloading();
};
