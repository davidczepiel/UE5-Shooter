// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	//Function to prepare the menu with the configuration for the desireg game the player is goind to search/host
	UFUNCTION(BlueprintCallable)	void MenuSetUp(int32 numPubConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//callbacks for the custom delegates on the MultiplayerSessionSubsystem
	UFUNCTION()	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccesful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION() void OnDestroySession(bool bWasSuccessful);
	UFUNCTION() void OnStartsession(bool bWasSuccessful);

private:
	//References to the menu buttons work with them if needed
	//meta is used to specify that in the widget ther is an existing button with the exact same name that can be tracked an used for these variables
	UPROPERTY(meta = (BindWidget)) class UButton* HostButton;
	UPROPERTY(meta = (BindWidget)) class UButton* JoinButton;

	//Functions that are going to be called when each menu button is clicked
	UFUNCTION()		void HostButtonClick();
	UFUNCTION()		void JoinButtonClick();

	//Method that is going to be called to clear all the menu related stuff because the player is going to join/host a session
	void MenuTearDown();

	//Plugin that is going to handle the creation, destruction, joining... of sessions
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	//Session configuration, number of players that can play simultaneously, the gamemode that is going to be played and the path to the map that the players are going to spawn in
	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };
};
