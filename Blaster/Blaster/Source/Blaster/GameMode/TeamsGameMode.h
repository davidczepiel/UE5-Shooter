// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API ATeamsGameMode : public ABlasterGameMode
{
	GENERATED_BODY()
public:
	ATeamsGameMode();
	/// <summary>
	/// Function that is called whenever a new player steps into the match
	/// </summary>
	/// <param name="NewPlayer"> New player that just connected to the lobby</param>
	virtual void PostLogin(APlayerController* NewPlayer) override;
	/// <summary>
	/// This function is called everytime a damage source connects with a character, it takes both the dealer and the receiver and calculates the amount of damages that the receiver needs to take
	/// </summary>
	/// <param name="Attacker"> Damage dealer </param>
	/// <param name="Victim"> Damage Receiver </param>
	/// <param name="BaseDamage"> Damage dealt </param>
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	/// <summary>
	/// This function is called each time a player is killed, it takes the atacker and the victim and updates their respective scores, aswell as the gamestate
	/// </summary>
	/// <param name="ElimCharacter"> Killer </param>
	/// <param name="VictimController"> Killed </param>
	/// <param name="Attacker5Controller"> Killer controller </param>
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) override;

	virtual void Logout(AController* Exiting) override;

protected:
	virtual void HandleMatchHasStarted() override;
};