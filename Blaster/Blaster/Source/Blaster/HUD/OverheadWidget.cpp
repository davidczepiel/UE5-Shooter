// Fill out your copyright notice in the Description page of Project Settings.

#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayedText(FString TextToDisplay)
{
	//IF the overhead text exists we display the text parameter on it
	if (DisplayText)	DisplayText->SetText(FText::FromString(TextToDisplay));
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	//Depending on the pawn net role we display one message or other
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole) {
		//THis pawn is playing on the server(he is the server)
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
		//This pawn is being controlled by the player but he is just a client (his computer is not acting as the server)
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous proxy");
		break;
		//This pawn represents the character of a client
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	//Name of the player that is controlling each pawn
	APlayerState* PlayerState = InPawn->GetPlayerState();
	FString PlayerName = PlayerState->GetPlayerName();
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s Name: %s "), *Role, *PlayerName);
	SetDisplayedText(RemoteRoleString);
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}