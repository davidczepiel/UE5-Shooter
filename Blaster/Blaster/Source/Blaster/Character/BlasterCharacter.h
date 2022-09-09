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
	virtual void Destroyed() override;

	/// <summary>
	/// Function that takes care of rotating the character
	/// </summary>
	/// <param name="DeltaTime"> Delta time to calculate the amount to rotate each tick </param>
	void RotateInPlace(float DeltaTime);
	/// <summary>
	/// Function that takes care of playing the fire animation
	/// </summary>
	/// <param name="bAiming"> Bool that says if the character is currently aiming or not </param>
	void PlayFireMontage(bool bAiming);
	/// <summary>
	/// Function that plays the reload animation
	/// </summary>
	void PlayReloadMontage();
	/// <summary>
	/// Function that plays a specific animation when the player dies
	/// </summary>
	void PlayElimMontage();
	/// <summary>
	/// Function that takes care of playing an animation when the player gets hit with a bullet or takes damage
	/// </summary>
	void PlayHitReactMontage();
	/// <summary>
	/// Function that sets this characters team
	/// </summary>
	/// <param name="Team"> Team that the character belongs to </param>
	void SetTeamColor(ETeam Team);

	//Movement and death
	virtual void OnRep_ReplicatedMovement() override;
	UPROPERTY(Replicated)		bool bDisableGameplay = false;

	/// <summary>
	/// Function that takes care of notifying other components to update the HUD health shown to the player
	/// </summary>
	void UpdateHUDHealth();
	/// <summary>
	/// Function that takes care of notifying other components to update the HUD shield shown to the player
	/// </summary>
	void UpdateHUDShield();

	/// <summary>
	/// RPC that is notifyed to all the players that makes this character play the death animation, it also sets the timer for respawning and the bool that determines if the player should respawn or leave the game
	/// </summary>
	/// <param name="bPlayerLeftGame"></param>
	/// <returns></returns>
	UFUNCTION(NetMulticast, Reliable)		void MulticastElim(bool bPlayerLeftGame);
	/// <summary>
	/// Function that takes care of dropping or destroying the equipped weapon and notifying the rest of the players by an RPC
	/// </summary>
	/// <param name="bPlayerLeftGame"> Bool that says if the player should look for another spawn point or if he should leave the game</param>
	void Elim(bool bPlayerLeftGame);

	/// <summary>
	/// Function that is called when input is pressed
	/// </summary>
	void FireButtonPressed();
	/// <summary>
	/// Function that takes care of a weapon that is overlapping with the character and renders its pickup widget
	/// </summary>
	/// <param name="Weapon"></param>
	void SetOverlappingWeapon(AWeapon* Weapon);
	/// <summary>
	/// Function that says if the character currently has a weapon equipped or not
	/// </summary>
	/// <returns></returns>
	bool IsWeaponEquipped();
	/// <summary>
	/// Function that says if the character currently is aiming or not
	/// </summary>
	/// <returns></returns>
	bool IsAiming();
	/// <summary>
	/// Function that takes care of the current rotation of the character and if it is higher than a specific value sets a couple of variables that will lead the character to play certain animations of rotating in place
	/// </summary>
	/// <param name="DeltaTime"></param>
	void TurnInplace(float DeltaTime);
	/// <summary>
	/// Returns the combat state
	/// </summary>
	/// <returns></returns>
	ECombatState GetCombatState();
	/// <summary>
	/// Returns the equipped weapon or nullptr if it is invalid
	/// </summary>
	/// <returns></returns>
	AWeapon* GetEquippedWeapon();
	/// <summary>
	/// Gets the hittarget saved in the combat component (place where the bullets whould go if shooted)
	/// </summary>
	/// <returns></returns>
	FVector GetHitTarget() const;

	/// <summary>
	/// Function that lets the player spawn with a default weapon equipped instead of being unarmed
	/// </summary>
	void SpawnDefaultWeapon();

	UPROPERTY()					class ABlasterPlayerState* BlasterPlayerState;

	/// <summary>
	/// RPC that notifies all the machines that this player just gained the lead and should have a crown on top of his head
	/// </summary>
	UFUNCTION(NetMulticast, Reliable)		void MulticastGainedTheLead();
	/// <summary>
	/// RPC that notifies all the machines that this player just lost the lead and should no longer have a crown on top of his head
	/// </summary>
	UFUNCTION(NetMulticast, Reliable)		void MulticastLostTheLead();
	UPROPERTY(EditAnywhere)		class UNiagaraSystem* CrownSystem;
	UPROPERTY()					class UNiagaraComponent* CrownComponent;

	//Hitboxes for server-side rewind
	UPROPERTY()					TMap<FName, class UBoxComponent*> HitCollisionBoxes;
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

	/// <summary>
	/// RPC that notifies the server that a player is going to leave the game and lets the server remove that player from the leaderboards and the game state
	/// </summary>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)				void ServerLeaveGame();
	FOnLeftGame OnLeftGame;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PollInit();
	/// <summary>
	/// Function that is going to be called when this character is hit whith a damage source (bullet/explosion)
	/// </summary>
	/// <param name="DamagedActor">Actor that is getting hurt (this one) </param>
	/// <param name="Damage"> Amount of damage that is going to inflict</param>
	/// <param name="DType"> Damage type </param>
	/// <param name="InstigatorController"> Controller of the character that is going to apply damage to this one </param>
	/// <param name="DamageCauser">  Other character that is going to inflict damage to this one </param>
	/// <returns></returns>
	UFUNCTION()		void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DType, AController* InstigatorController, AActor* DamageCauser);

	/// <summary>
	/// FUnction called when the forward and backward keys are pressed
	/// </summary>
	/// <param name="Value"> Value of the movement amount that the character should move </param>
	virtual void MoveForward(float Value);
	/// <summary>
	/// FUnction called when the right and left keys are pressed
	/// </summary>
	/// <param name="Value"> Value of the movement amount that the character should move </param>
	void MoveRight(float Value);
	/// <summary>
	/// Function called when the player moves his mouse to rotate the character (to the sides)
	/// </summary>
	/// <param name="Value"> Value of the rotation amount that the character should rotated </param>
	void Turn(float Value);
	/// <summary>
	/// Function that is called when the player moves his mouse to rotate the character (up and down)
	/// </summary>
	/// <param name="Value"> Value of the rotation amount that the character should rotated</param>
	void LookUp(float Value);
	/// <summary>
	/// Function that is going to be called when the jump button is pressed
	/// </summary>
	virtual void Jump() override;

	////////////////////Functions mapped to specific input///////////////////
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

	/// <summary>
	/// Function that communicates components related to both hud and weapons and takes care of updating the ammo showed in the HUD
	/// </summary>
	void UpdateHUDAmmo();

private:
	/// <summary>
	/// Function that makes this character invisible if the case that it gets to close to it
	/// </summary>
	void HideCameraIfCharacterClose();
	UPROPERTY(VisibleAnywhere, Category = Camera)											class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)											class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere)																	float CameraThreshHold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))		class UWidgetComponent* OverheadWidget;

	UFUNCTION()		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)									class AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))		class UCombatComponent* Combat;
	UPROPERTY(VisibleAnywhere)																class UBuffComponent* Buff;
	UPROPERTY(VisibleAnywhere)																class ULagCompensationComponent* LagCompensation;

	/// <summary>
	/// RPC that notifyes the server that a weapon must be equipped (the updated value will replicate to the rest of the machines)
	/// </summary>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)		void ServerEquipButtonPress();

	/// <summary>
	/// Returns the speed of the character
	/// </summary>
	/// <returns></returns>
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
	UPROPERTY(EditAnywhere, Category = "Player Stats")										float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "Player Stats")		float CurrentHealth = 100.f;

	UFUNCTION()		void OnRep_Shield(float lastShield);
	UPROPERTY(EditAnywhere, Category = "Player Stats")										float MaxShield = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")		float CurrentShield = 25.f;

	bool bLeftGame = false;

	/// <summary>
	/// Function that gets called when the respawn timer reaches 0, it takes care of respawning the character in another map position or kicking him out of the game
	/// </summary>
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)		float ElimDelay = 3.f;
	FTimerHandle ElimTimer;
	bool bElim = false;

	UPROPERTY()	class ABlasterPlayerController* BlasterPlayerController;

	/// <summary>
	/// Function that starts the dissolve material "animation"
	/// </summary>
	void StartDissolve();
	/// <summary>
	/// Callback that gets called when playing the dissolve animation and updates the dissolve parameters
	/// </summary>
	/// <param name="DissolveValue"></param>
	/// <returns></returns>
	UFUNCTION()		void UpdateDissolveMaterial(float DissolveValue);
	UPROPERTY(VisibleAnywhere)						UTimelineComponent* DissolveTimeline;
	UPROPERTY(EditAnywhere)							UCurveFloat* DissolveCurve;
	UPROPERTY(VisibleAnywhere, Category = Elim)		UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* DissolveMaterialInstance;
	FOnTimelineFloat DissolveTrack;

	/**
* Team colors
*/

	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* RedDissolveMatInst;
	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* RedMaterial;
	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* BlueDissolveMatInst;
	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* BlueMaterial;
	UPROPERTY(EditAnywhere, Category = Elim)		UMaterialInstance* OriginalMaterial;

	//DefaultWeapon
	UPROPERTY(EditAnywhere)		TSubclassOf<AWeapon> DefaultWeapon;

	UPROPERTY()		class ABlasterGameMode* BlasterGameMode;

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
