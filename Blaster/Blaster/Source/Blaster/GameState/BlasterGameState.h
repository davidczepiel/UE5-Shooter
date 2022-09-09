// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/// <summary>
	/// This function es called whenever a player scores a kill to see if he is now the new leading player
	/// </summary>
	/// <param name="ScoringPlayer"> Player that just scored a kill </param>
	void UpdateTopScore(class ABlasterPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)		TArray<ABlasterPlayerState*> TopScoringPlayers;

	/// <summary>
	/// THis function is called whenever a player from the red team scores
	/// </summary>
	void RedTeamScores();
	/// <summary>
	/// THis function is called whenever a player from the red team scores
	/// </summary>
	void BlueTeamScores();

	TArray<ABlasterPlayerState*> RedTeam;
	TArray<ABlasterPlayerState*> BlueTeam;
	UFUNCTION()		void OnRep_RedTeamScore();
	UFUNCTION()		void OnRep_BlueTeamScore();
	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)			float RedTeamScore = 0.f;
	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)		float BlueTeamScore = 0.f;
private:
	float TopScore = 0.f;
};