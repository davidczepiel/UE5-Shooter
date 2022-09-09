// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState {
	extern BLASTER_API const FName Cooldown; //Match duration reached. Display winner and details
}

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// This function is called each time a player is killed, it takes the atacker and the victim and updates their respective scores, aswell as the gamestate
	/// </summary>
	/// <param name="ElimCharacter"> Killer </param>
	/// <param name="VictimController"> Killed </param>
	/// <param name="Attacker5Controller"> Killer controller </param>
	virtual void PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller);
	/// <summary>
	/// This function is called everytime a character is about to respawn, it takes the victim components and it takes care of everything to make him respawn at a new location
	/// </summary>
	/// <param name="ElimCharacter"> Victim character </param>
	/// <param name="ElimController"> Victim Controller </param>
	/// <returns></returns>
	virtual bool RequestRespawn(class ACharacter* ElimCharacter, class AController* ElimController);

	/// <summary>
	/// This function is called everytime a player leaves the game, it removes the player from the leaderboards
	/// </summary>
	/// <param name="PlayerLeaving"> State of the leaving player </param>
	void PlayerLeftGame(class ABlasterPlayerState* PlayerLeaving);
	/// <summary>
	/// This function is called everytime a damage source connects with a character, it takes both the dealer and the receiver and calculates the amount of damages that the receiver needs to take
	/// </summary>
	/// <param name="Attacker"> Damage dealer </param>
	/// <param name="Victim"> Damage Receiver </param>
	/// <param name="BaseDamage"> Damage dealt </param>
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	//Time related variables
	UPROPERTY(EditDefaultsOnly)		float WarmupTime = 10.f;
	UPROPERTY(EditDefaultsOnly)		float MatchTime = 120.f;
	UPROPERTY(EditDefaultsOnly)		float CooldownTime = 10.f;
	float LevelStartingTime = 0.f;
	bool bTeamsMatch = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountDownTime = 0.f;
public:
	FORCEINLINE float GetCountdownTime() { return CountDownTime; }
};
