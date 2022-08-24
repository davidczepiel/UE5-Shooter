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
		//If timer ended while waiting, a new match is going to start
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime < 0.f)			StartMatch();
	}
	else if (MatchState == MatchState::InProgress) {
		//If the timer ends while playing a match, means the match has come to its end
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)			SetMatchState(MatchState::Cooldown);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		//If in cooldown and the timer has ended, a restart is needed and a new game is played
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)			RestartGame();
	}
}

void ABlasterGameMode::OnMatchStateSet() {
	Super::OnMatchStateSet();

	//Every controller from the world is fetched and is notified with the match state the game is in
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		ABlasterPlayerController* p = Cast<ABlasterPlayerController>(*it);
		if (p) 	p->OnMatchStateSet(MatchState);
	}
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller) {
	//Players involved int he kill
	ABlasterPlayerState* AtState = Attacker5Controller ? Cast<ABlasterPlayerState>(Attacker5Controller->PlayerState) : nullptr;
	ABlasterPlayerState* VictimState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	//The game state is obtained and the kills/deaths are updated
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