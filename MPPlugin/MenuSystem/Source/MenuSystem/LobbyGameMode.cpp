// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GameState) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(1, 6.f, FColor::Yellow, FString::Printf(TEXT("Player count %d"), NumberOfPlayers));
			APlayerState* NewPlayerState = NewPlayer->GetPlayerState<APlayerState>();
			FString PlayerName;
			if (NewPlayerState) {
				PlayerName = NewPlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(1, 6.f, FColor::Cyan, FString::Printf(TEXT("%s has joined"), *PlayerName));
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (GameState) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(1, 6.f, FColor::Yellow, FString::Printf(TEXT("Player count %d"), NumberOfPlayers - 1));
			APlayerState* NewPlayerState = Exiting->GetPlayerState<APlayerState>();
			FString PlayerName;
			if (NewPlayerState) {
				PlayerName = NewPlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(1, 6.f, FColor::Cyan, FString::Printf(TEXT("%s has exited"), *PlayerName));
			}
		}
	}
}