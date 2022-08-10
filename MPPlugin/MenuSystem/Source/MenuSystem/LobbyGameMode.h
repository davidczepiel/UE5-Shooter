// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 *
 */
UCLASS()
class MENUSYSTEM_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//MEthod called when a new player has joined the session
	virtual void PostLogin(APlayerController* NewPlayer) override;
	//MEthod called when a new player exits the session
	virtual void Logout(AController* Exiting) override;
};
