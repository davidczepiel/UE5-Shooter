// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	virtual void OnRep_Score() override;
	UFUNCTION()	virtual void OnRep_Defeats();

	/// <summary>
	/// Function that gets called whenever a player scores a kill and points need to be added to its state
	/// </summary>
	/// <param name="delta"> Amount of points to be added </param>
	void AddToScore(float delta);
	/// <summary>
	/// Function that gets called whenever a player is killed
	/// </summary>
	/// <param name="delta"> Amount of deaths that need to be added </param>
	void AddToDefeats(int32 delta);

private:
	UFUNCTION()		void OnRep_Team();

	UPROPERTY()									class ABlasterCharacter* Character;
	UPROPERTY()									class ABlasterPlayerController* Controller;
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)	int32 Defeats;
	UPROPERTY(ReplicatedUsing = OnRep_Team)		ETeam Team = ETeam::ET_NoTeam;

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
};
