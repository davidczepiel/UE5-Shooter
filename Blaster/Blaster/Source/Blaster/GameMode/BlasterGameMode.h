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
	virtual void PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller);
	virtual bool RequestRespawn(class ACharacter* ElimCharacter, class AController* ElimController);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		float WarmupTime = 10.f;
	UPROPERTY(EditDefaultsOnly)
		float MatchTime = 120.f;
	UPROPERTY(EditDefaultsOnly)
		float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountDownTime = 0.f;
public:
	FORCEINLINE float GetCountdownTime() { return CountDownTime; }
};
