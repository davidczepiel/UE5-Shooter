// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, Team);
}

void ABlasterPlayerState::OnRep_Score() {
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	//if a character is avaliable the score in the HUD is updated
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)		Controller->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	//if a character is avaliable the defeats in the HUD are updated
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::AddToScore(float delta) {
	SetScore(GetScore() + delta);

	//HUD update
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)		Controller->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::AddToDefeats(int32 delta) {
	//HUD update
	Defeats += delta;
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character) {
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	ABlasterCharacter* BCharacter = Cast <ABlasterCharacter>(GetPawn());
	if (BCharacter)	BCharacter->SetTeamColor(Team);
}

void ABlasterPlayerState::OnRep_Team()
{
	ABlasterCharacter* BCharacter = Cast <ABlasterCharacter>(GetPawn());
	if (BCharacter)	BCharacter->SetTeamColor(Team);
}