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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/// <summary>
	/// Checks if the ping this player has is too high, if it is, it starts to display a high ping warning on the HUD
	/// </summary>
	/// <param name="DeltaTime"></param>
	void CheckPing(float DeltaTime);
	/// <summary>
	/// Function that gets called whenever this controller gets another character to "control"
	/// </summary>
	/// <param name="pawn"></param>
	void OnPossess(APawn* pawn) override;

	/// <summary>
	/// Funciton that takes care of sincronizing the time with the server every x amount of seconds
	/// </summary>
	/// <param name="DeltaTime"></param>
	void CheckTimeSync(float DeltaTime);
	/// <summary>
	/// Gets the match time taking in consideration wether this player is the server or a client
	/// </summary>
	virtual float GetServerTime();

	/// <summary>
	/// Set this players health on the HUD
	/// </summary>
	/// <param name="Health">Health</param>
	/// <param name="MaxHealth">MaxHealth</param>
	void SetHUDHelth(float Health, float MaxHealth);
	/// <summary>
	/// Sets this players shield on the HUD
	/// </summary>
	/// <param name="shield"> shield</param>
	/// <param name="MaxShield"> MaxShield</param>
	void SetHUDShield(float shield, float MaxShield);
	/// <summary>
	/// Sets this players score
	/// </summary>
	/// <param name="Score"> Score </param>
	void SetHUDScore(float Score);
	/// <summary>
	/// Sets the amount of defeats this player has had
	/// </summary>
	/// <param name="Defeats"> Defeats </param>
	void SetHUDDefeats(int32 Defeats);
	/// <summary>
	/// Sets the magacine ammo on the HUD
	/// </summary>
	/// <param name="ammo"> Magacine amount </param>
	void SetHUDWeaponAmmo(int32 ammo);
	/// <summary>
	/// Sets the character ammo on the HUD
	/// </summary>
	/// <param name="ammo"> Ammo carried amount </param>
	void SetHUDCarriedAmmo(int32 ammo);
	/// <summary>
	/// Set the hud countdown number during a match
	/// </summary>
	/// <param name="CountDown"></param>
	void SetHUDMatchCountdown(float CountDown);
	/// <summary>
	/// Sets the HUD warmup/countdown number both before and after the match
	/// </summary>
	void SetHUDAnouncementCountDown(float CountDown);

	/// <summary>
	/// Sets the HUD time
	/// </summary>
	void SetHUDTime();

	/// <summary>
	/// Method for clients to ask the server for the server time
	/// </summary>
	virtual void ReceivedPlayer() override;
	/// <summary>
	/// Sets the match state to a given value and sets the teams bool
	/// </summary>
	/// <param name="State"> Match state to enter </param>
	/// <param name="bTeamsMatch"> This match is with teams or free4all </param>
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);

	/// <summary>
	/// Prepares the HUD to show the elements needed on the start of a match
	/// </summary>
	void HandleMatchHasStarted(bool bTeamsMatch = false);

	/// <summary>
	/// Prepares the HUD to show the elements needed on the end of a match
	/// </summary>
	void HandleCooldown();

	float SingleTripTime = 0.f;
	FHighPingDelegate HighPingDelegate;

	/// <summary>
	/// Calls an RPC for clients to anounce them that a kill was confirmed
	/// </summary>
	/// <param name="Attacker"> Killer </param>
	/// <param name="Victim"> Killed </param>
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	/// <summary>
	/// Hides the team scores form the hide
	/// </summary>
	void HideTeamScores();
	/// <summary>
	/// Sets the team scores with default values in the HUD
	/// </summary>
	void InitTeamScores();

	/// <summary>
	/// Sets the red team score to a specific value
	/// </summary>
	/// <param name="RedScore"> red team score </param>
	void SetHUDRedTeamScore(int32 RedScore);
	/// <summary>
	/// Sets the blue team score to a specific value
	/// </summary>
	/// <param name="BlueScore"> Blue team score </param>
	void SetHUDBlueTeamScore(int32 BlueScore);

protected:
	virtual void BeginPlay() override;
	void PollInit();

	/// <summary>
	/// Sets the esc key to open the exit to main menu menu
	/// </summary>
	virtual void SetupInputComponent() override;

	/// <summary>
	/// RPC to the server that allows the clients to ask him for match data like the match time left...
	/// </summary>
	UFUNCTION(Server, Reliable)		void ServerCheckMatchState();
	/// <summary>
	/// RPC that ther server recieves to send a client information to fetch data from the time delta betwean the clients game and the servers game
	/// </summary>
	/// <param name="TimeOfClientRequest"> Time when the client sent the request </param>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)		void ServerRequestServerTime(float TimeOfClientRequest);

	/// <summary>
	/// Callback for the client that lets fim know te amount of time it took for the request to both get to ther server and come back to the client
	/// </summary>
	/// <param name="TimeOfClientRequest"> Time when the client requested the time </param>
	/// <param name="TimeServerReceivedRequest"> Time when the server actually answered </param>
	/// <returns></returns>
	UFUNCTION(Client, Reliable)		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);
	/// <summary>
	/// This RPC is sent to clients whenever they join a game midsession and sends them the information about the match that they need to display on their HUD
	/// </summary>
	/// <param name="state"> State of the match </param>
	/// <param name="warmup"> Time left of the warm up state </param>
	/// <param name="match"> Match time left </param>
	/// <param name="starttime">Match start time </param>
	/// <param name="cooldown"> Cooldown time left after match ended</param>
	/// <returns></returns>
	UFUNCTION(Client, Reliable)		void ClientJoinMidGame(FName state, float warmup, float match, float starttime, float cooldown);

	UPROPERTY(EditAnywhere, Category = Time)		float TimeSyncFrequency = 5.f;
	float ClientServerDelta = 0.f;
	float TimeSyncRunningTime = 0.f;

	/// <summary>
	/// THis function plays an animation that shows on the HUD that the players ping is not ideal
	/// </summary>
	void HighPingWarning();

	/// <summary>
	/// THis function stops/hides the high ping animation
	/// </summary>
	void  StopHighPingWarning();

	/// <summary>
	/// FUnction called from another component when the player presses the esc key and wants to go to the main menu, this function shows a menu that allows it
	/// </summary>
	void ShowReturnToMainMenu();

	/// <summary>
	/// RPC for clients that notifies them that a kill was performed on the server
	/// </summary>
	/// <param name="Attacker"> Killer </param>
	/// <param name="Victim"> Killed </param>
	UFUNCTION(Client, Reliable)		void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	UFUNCTION()		void OnRep_ShowTeamScores();
	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)		bool bShowTeamScores = false;

	/// <summary>
	/// Function that creates a text iforming about the winner/s of the match
	/// </summary>
	/// <param name="BlasterGameState"> Text that shows the winer </param>
	/// <returns></returns>
	FString GetInfoText(const TArray<class ABlasterPlayerState*>& Players);

	/// <summary>
	/// Function that creates a text iforming about the match result, who won, if it was a tie...
	/// </summary>
	/// <param name="BlasterGameState"> Text that shows the winer </param>
	/// <returns></returns>
	FString GetTeamsInfoText(class ABlasterGameState* BlasterGameState);
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
	UPROPERTY(EditAnywhere, Category = HUD)		TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;
	UPROPERTY()									class UReturnToMainMenu* ReturnToMainMenu;
	bool bReturnToMainMenuOpen = false;

	//Ping
	UFUNCTION(Server, Reliable)		void ServerReportPingStatus(bool bHighPing);
	UPROPERTY(EditAnywhere)		float HighPingRunningTime = 0.f;
	UPROPERTY(EditAnywhere)		float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)		float CheckPingFrequency = 5.f;
	UPROPERTY(EditAnywhere)		float HighPingThreshold = 50.f;
	UPROPERTY(EditAnywhere)		float PingAnimationRunningTime = 0.f;
};
