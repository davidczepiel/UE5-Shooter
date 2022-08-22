// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Anouncement.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API UAnouncement : public UUserWidget
{
public:
	GENERATED_BODY()
		UPROPERTY(meta = (BindWidget))
		class UTextBlock* WarmUpTime;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AnouncementText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* InfoText;
};
