// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// This function is called to specify the text that needs to be displayed on top of a characters head
	/// </summary>
	void SetDisplayedText(FString TextToDisplay);

	UPROPERTY(meta = (BindWidget))		class UTextBlock* DisplayText;
	UFUNCTION(BlueprintCallable)		void ShowPlayerNetRole(APawn* InPawn);
protected:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
