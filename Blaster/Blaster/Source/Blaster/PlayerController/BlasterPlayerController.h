// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnPossess(APawn* pawn) override;
	void CheckTimeSync(float DeltaTime);
	virtual float GetServerTime();

	//HUD Methods
	void SetHUDHelth(float Health, float MaxHealth);
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

protected:
	virtual void BeginPlay() override;
	void HandleMatchHasStarted();
	void PollInit();

	//Sync time between client and server
	UFUNCTION(Server, Reliable)		void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Server, Reliable)		void ServerCheckMatchState();
	UFUNCTION(Server, Reliable)		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);
	UFUNCTION(Client, Reliable)		void ClientJoinMidGame(FName state, float warmup, float match, float starttime, float cooldown);

	UPROPERTY(EditAnywhere, Category = Time)		float TimeSyncFrequency = 5.f;
	float ClientServerDelta = 0.f;
	float TimeSyncRunningTime = 0.f;

private:
	//Character overhead overlay
	UPROPERTY()	class UCharacterOverlay* CharacterOverlay;

	//Match related stuff
	UFUNCTION()										void OnRep_MatchState();
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)	FName MatchState;
	UPROPERTY()										class ABlasterGameMode* GameMode;

	//HUD related variables
	UPROPERTY()	class ABlasterHUD* BlasterHUD;
	bool bInitCharacterOverlay = false;
	float HUDHealth;
	float HUDMaxHealth;
	float matchtime = 120.f;
	float WarmUptime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	float LevelStartingTime = 0.f;
	int32 HUDDefeats;
	int32 HUDScore;
};
