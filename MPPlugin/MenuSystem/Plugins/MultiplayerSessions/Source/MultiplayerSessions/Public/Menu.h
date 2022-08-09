// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void MenuSetUp(int32 numPubConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//callbacks for the custom delegates on the MultiplayerSessionSubsystem
	UFUNCTION()
		void OnCreateSession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget)) class UButton* HostButton;
	UPROPERTY(meta = (BindWidget)) class UButton* JoinButton;

	UFUNCTION()		void HostButtonClick();
	UFUNCTION()		void JoinButtonClick();

	void MenuTearDown();

	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
};
