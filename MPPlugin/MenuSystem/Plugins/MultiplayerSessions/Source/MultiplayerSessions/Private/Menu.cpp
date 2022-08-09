// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"

void UMenu::MenuSetUp() {
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

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
}