// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime);
	void CheckPing(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnPossess(APawn* pawn) override;
	void CheckTimeSync(float DeltaTime);
	virtual float GetServerTime();

	//HUD Methods
	void SetHUDHelth(float Health, float MaxHealth);
	void SetHUDShield(float shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 ammo);
	void SetHUDCarriedAmmo(int32 ammo);
	void SetHUDMatchCountdown(float CountDown);
	void SetHUDAnouncementCountDown(float CountDown);
	void SetHUDTime();

	//Match related methods
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName state);
	void HandleCooldown();

	float SingleTripTime = 0.f;
	FHighPingDelegate HighPingDelegate;

	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

protected:
	virtual void BeginPlay() override;
	void HandleMatchHasStarted();
	void PollInit();

	virtual void SetupInputComponent() override;

	//Sync time between client and server
	UFUNCTION(Server, Reliable)		void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Server, Reliable)		void ServerCheckMatchState();
	UFUNCTION(Server, Reliable)		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);
	UFUNCTION(Client, Reliable)		void ClientJoinMidGame(FName state, float warmup, float match, float starttime, float cooldown);

	UPROPERTY(EditAnywhere, Category = Time)		float TimeSyncFrequency = 5.f;
	float ClientServerDelta = 0.f;
	float TimeSyncRunningTime = 0.f;

	void HighPingWarning();
	void  StopHighPingWarning();

	void ShowReturnToMainMenu();

	UFUNCTION(Client, Reliable)
		void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);
private:
	//Character overhead overlay
	UPROPERTY()	class UCharacterOverlay* CharacterOverlay;

	//Match related stuff
	UFUNCTION()										void OnRep_MatchState();
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)	FName MatchState;
	UPROPERTY()										class ABlasterGameMode* GameMode;

	//HUD related variables
	UPROPERTY()	class ABlasterHUD* BlasterHUD;
	bool bInitHealth = false;
	bool bInitShield = false;
	bool bInitScore = false;
	bool bInitDefeats = false;
	bool bInitWeaponAmmo = false;
	bool bInitCarriedAmmo = false;
	float HUDCarriedAmmo;
	float HUDWeaponAmmo;
	float HUDHealth;
	float HUDMaxHealth;
	float HUDShield;
	float HUDMaxShield;
	float matchtime = 120.f;
	float WarmUptime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	float LevelStartingTime = 0.f;
	int32 HUDDefeats;
	int32 HUDScore;

	/**
* Return to main menu
*/

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
		class UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	//Ping
	UPROPERTY(EditAnywhere)
		float HighPingRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
		float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
		float CheckPingFrequency = 5.f;

	UFUNCTION(Server, Reliable)
		void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
		float HighPingThreshold = 50.f;
	UPROPERTY(EditAnywhere)
		float PingAnimationRunningTime = 0.f;
};
