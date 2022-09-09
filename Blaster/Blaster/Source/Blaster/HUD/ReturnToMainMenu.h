// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	/// <summary>
	/// Function to add the widget to the players screan
	/// </summary>
	void MenuSetup();
	/// <summary>
	/// Function to remove this widget from the players screen
	/// </summary>
	void MenuTearDown();

protected:
	virtual bool Initialize() override;

	UFUNCTION()		void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()		void OnPlayerLeftGame();

private:
	/// <summary>
	/// THis callback is called whenever the button to go back to the main menu is clicked
	/// </summary>
	UFUNCTION()		void ReturnButtonClicked();

	UPROPERTY(meta = (BindWidget))		class UButton* ReturnButton;
	UPROPERTY()							class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	UPROPERTY()							class APlayerController* PlayerController;
};