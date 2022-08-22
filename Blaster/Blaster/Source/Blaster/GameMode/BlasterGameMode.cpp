// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"

namespace MatchState {
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode() {
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay() {
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart) {
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime < 0.f) {
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress) {
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f) {
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet() {
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		ABlasterPlayerController* p = Cast<ABlasterPlayerController>(*it);
		if (p) {
			p->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller) {
	ABlasterPlayerState* AtState = Attacker5Controller ? Cast<ABlasterPlayerState>(Attacker5Controller->PlayerState) : nullptr;
	ABlasterPlayerState* VictimState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if (AtState && AtState != VictimState && BlasterGameState) {
		AtState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AtState);
	}
	if (VictimState) {
		VictimState->AddToDefeats(1);
	}
	if (ElimCharacter) {
		ElimCharacter->Elim();
	}
}

bool ABlasterGameMode::RequestRespawn(ACharacter* ElimCharacter, AController* ElimController) {
	if (ElimCharacter) {
		ElimCharacter->Reset();
		ElimCharacter->Destroy();
	}
	if (ElimController) {
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimController, PlayerStarts[Selection]);
	}

	return true;
}