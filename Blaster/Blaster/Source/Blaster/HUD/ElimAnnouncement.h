// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncement.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
	/// <summary>
	/// This function is called to specify the text that needs to be displayed at the users HUD
	/// </summary>
	void SetElimAnnouncementText(FString AttackerName, FString VictimName);

	UPROPERTY(meta = (BindWidget))		class UHorizontalBox* AnnouncementBox;
	UPROPERTY(meta = (BindWidget))		class UTextBlock* AnnouncementText;
};