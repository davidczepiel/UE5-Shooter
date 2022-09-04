// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	//Health and shield
	UPROPERTY(meta = (BindWidget))		class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))		class UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))		UProgressBar* ShieldBar;
	UPROPERTY(meta = (BindWidget))		UTextBlock* ShieldText;

	//Weapon
	UPROPERTY(meta = (BindWidget))		class UTextBlock* WeaponAmmoAmount;
	UPROPERTY(meta = (BindWidget))		class UTextBlock* CarriedAmmoAmount;

	//High ping HUD
	UPROPERTY(meta = (BindWidget))	 class UImage* HighPingImage;
	UPROPERTY(meta = (BindWidgetAnim), Transient)UWidgetAnimation* HighPingAnimation;

	//Team scores
	UPROPERTY(meta = (BindWidget))		UTextBlock* RedTeamScore;
	UPROPERTY(meta = (BindWidget))		UTextBlock* BlueTeamScore;
	UPROPERTY(meta = (BindWidget))		UTextBlock* ScoreSpacerText;

	//Match
	UPROPERTY(meta = (BindWidget))		class UTextBlock* MatchCountDownText;
	UPROPERTY(meta = (BindWidget))		class UTextBlock* ScoreAmount;
	UPROPERTY(meta = (BindWidget))		class UTextBlock* DefeatsAmount;
};
