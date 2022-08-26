// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

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

	//Movement and death
	virtual void OnRep_ReplicatedMovement() override;
	virtual void Destroyed() override;
	UPROPERTY(Replicated)		bool bDisableGameplay = false;
	void UpdateHUDHealth();

	//Player was eliminated
	UFUNCTION(NetMulticast, Reliable)		void MulticastElim();
	void Elim();

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
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")		float CurrentHealth = 100.f;

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

public:
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE float GetHealth() const { return CurrentHealth; }
	FORCEINLINE void SetHealth(float amount) { CurrentHealth = amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool IsElimmed() const { return bElim; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
};
