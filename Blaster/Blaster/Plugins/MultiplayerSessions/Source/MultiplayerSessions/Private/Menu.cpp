// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetUp(int32 numPubConnections, FString TypeOfMatch, FString LobbyPath)
{
	//Class variables
	NumPublicConnections = numPubConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	//Basic methods to prepare the menu widget
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	//Input mode is changed so that the player can use the menu's buttons correctly
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModedata;
			InputModedata.SetWidgetToFocus(TakeWidget());
			InputModedata.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModedata);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	//The multiplayer session "manager" is obtained so that it can be called later when the player attempts ot host/join sessions
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	//Callbacks are prepared for the different actions the player can do in the menu (host/join/create... sessions)
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartsession);
	}
}

bool UMenu::Initialize()
{
	//Parent component stuff
	if (!Super::Initialize()) return false;

	//If the menu buttons are avaliable, their onClick callbacks are specified to point to methods from this class
	if (HostButton) HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClick);
	if (JoinButton) JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClick);

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	//If the session was created successfuly the player is moved to the new world's session
	if (bWasSuccessful) {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Session Created successfully")));
		UWorld* World = GetWorld();
		if (World) World->ServerTravel(PathToLobby);
	}
	//If not, a message is displayed and the button to create a session is enabled
	else {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Session could not be created successfully")));
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccesful)
{
	if (MultiplayerSessionsSubsystem == nullptr) return;

	//The list of sessions found is analized and if a session with the matchtype the player is searching is found, he is automatically sent to it
	for (auto Result : SessionResults) {
		FString Settingsvalue;
		Result.Session.SessionSettings.Get(FName("MatchType"), Settingsvalue);
		if (Settingsvalue == MatchType) {
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	//If no sessions were found or the operation couldnt be done the find session button is enabled again
	if (!bWasSuccesful || SessionResults.Num() == 0)
		JoinButton->SetIsEnabled(true);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem) {
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			//The address of the session is obtained
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			//The player is sent to that address
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController) PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
	//If something went wrong during the operation the join button is enabled again
	if (Result != EOnJoinSessionCompleteResult::Success)
		JoinButton->SetIsEnabled(true);
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartsession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClick()
{
	//Button is disabled to prevent click spamming and undefined behaviour
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem) MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

void UMenu::JoinButtonClick()
{
	//Button is disabled to prevent click spamming and undefined behaviour
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem) MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenu::MenuTearDown()
{
	//The menu widget is removed from the screen
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		//The player gets back it's normal input configuration and can continue playing as expected
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly FInputModeData;
			PlayerController->SetInputMode(FInputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}