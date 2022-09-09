// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/Character/BlasterCharacter.h"

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

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	//THis mode represents a Free4All match so every source of damage needs to be applied its 100%
	return BaseDamage;
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
		ABlasterPlayerController* player = Cast<ABlasterPlayerController>(*it);
		if (player) {
			player->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller) {
	//Players involved int he kill
	ABlasterPlayerState* AtState = Attacker5Controller ? Cast<ABlasterPlayerState>(Attacker5Controller->PlayerState) : nullptr;
	ABlasterPlayerState* VictimState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	//The game state is obtained and the kills/deaths are updated
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (AtState && AtState != VictimState && BlasterGameState) {
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		//Both the player and gamestate scores are updated
		AtState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AtState);

		//If after updating the top scoring players the attacker still is on the lead it needs to be notified to everyone (create crowns)
		if (BlasterGameState->TopScoringPlayers.Contains(AtState))
		{
			ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AtState->GetPawn());
			if (Leader)	Leader->MulticastGainedTheLead();
		}

		//If any of the previous leaders is not currently in the lead it is notified to everyone (remove crowns)
		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)	Loser->MulticastLostTheLead();
			}
		}
	}

	//The kill is confirmed
	if (VictimState) 		VictimState->AddToDefeats(1);
	if (ElimCharacter) 		ElimCharacter->Elim(false);

	//THe kill is broadcasted to all the players so that they can see it on the killfeed
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AtState && VictimState)
		{
			BlasterPlayer->BroadcastElim(AtState, VictimState);
		}
	}
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	//The leaving player is removed from the score board
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))	BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);

	//The leaving player is killed with a true parameter to specify that he is leaving the game
	ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)	CharacterLeaving->Elim(true);
}

bool ABlasterGameMode::RequestRespawn(ACharacter* ElimCharacter, AController* ElimController) {
	//THe character is killed and destroyed
	if (ElimCharacter) {
		ElimCharacter->Reset();
		ElimCharacter->Destroy();
	}
	//The controller is respawned at another position
	if (ElimController) {
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimController, PlayerStarts[Selection]);
	}

	return true;
}