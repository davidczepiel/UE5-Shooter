// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage {
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCenter;
	class UTexture2D* CrosshairLeft;
	class UTexture2D* CrosshairRight;
	class UTexture2D* CrosshairTop;
	class UTexture2D* CrosshairBottom;

	float CrosshairSpread;

	FLinearColor CrosshairsColor;
};

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	void AddCharacterOverlay();
	void AddAnouncement();

	//Type of overlays that are going to be used
	UPROPERTY(EditAnywhere, Category = "Player Stats")		TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()												class UCharacterOverlay* CharacterOverlay;
	UPROPERTY(EditAnywhere, Category = "Announcements")		TSubclassOf<UUserWidget> AnouncementClass;
	UPROPERTY()												class UAnouncement* Anouncement;

protected:
	virtual void BeginPlay() override;

private:

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor color);

	UPROPERTY(EditAnywhere)		float CrosshairSpreadMax = 16.f;
	FHUDPackage HUDPackage;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
