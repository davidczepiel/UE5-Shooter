// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller) {
	if (ElimCharacter) {
		ElimCharacter->Elim();
	}
}