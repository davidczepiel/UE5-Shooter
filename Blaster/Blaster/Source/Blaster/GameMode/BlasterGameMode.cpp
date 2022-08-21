// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* Attacker5Controller) {
	ABlasterPlayerState* AtState = Attacker5Controller ? Cast<ABlasterPlayerState>(Attacker5Controller->PlayerState) : nullptr;
	ABlasterPlayerState* VictimState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AtState && AtState != VictimState) {
		AtState->AddToScore(1.f);
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