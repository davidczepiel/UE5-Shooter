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
	void SetHUDHelth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 ammo);
	void SetHUDCarriedAmmo(int32 ammo);
	void SetHUDMatchCountdown(float CountDown);
	void SetHUDAnouncementCountDown(float CountDown);
	void OnPossess(APawn* pawn) override;
	void SetHUDTime();

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	virtual void Tick(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CheckTimeSync(float DeltaTime);

	void OnMatchStateSet(FName state);
	void HandleCooldown();

protected:
	virtual void BeginPlay() override;
	void HandleMatchHasStarted();
	void PollInit();

	//Sync time between client and server

	UFUNCTION(Server, Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Server, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
		float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;

	UFUNCTION(Server, Reliable)
		void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
		void ClientJoinMidGame(FName state, float warmup, float match, float starttime, float cooldown);
private:
	UPROPERTY()
		class ABlasterHUD* BlasterHUD;

	float matchtime = 120.f;
	float WarmUptime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	float LevelStartingTime = 0.f;
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
		FName MatchState;
	UFUNCTION()
		void OnRep_MatchState();

	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	UPROPERTY()
		class ABlasterGameMode* GameMode;

	bool bInitCharacterOverlay = false;
	float HUDHealth;
	float HUDMaxHealth;
	int32 HUDDefeats;
	int32 HUDScore;
};
