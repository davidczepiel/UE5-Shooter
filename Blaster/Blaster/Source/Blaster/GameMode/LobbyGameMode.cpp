// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);

		//Number of players currently in this session
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		//If there are already the specified number of players connected the game can start
		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			UWorld* World = GetWorld();
			if (World) {
				bUseSeamlessTravel = true;
				//Depending on what the subsystem has stored the players traves to one map or another
				FString MatchType = Subsystem->DesiredMatchType;
				if (MatchType == "FreeForAll")			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
				else if (MatchType == "Teams")			World->ServerTravel(FString("/Game/Maps/Teams?listen"));
				else if (MatchType == "CaptureTheFlag")	World->ServerTravel(FString("/Game/Maps/CaptureTheFlag?listen"));
			}
		}
	}
}