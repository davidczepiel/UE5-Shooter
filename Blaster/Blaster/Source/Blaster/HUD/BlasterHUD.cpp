// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"

void ABlasterHUD::DrawHUD() {
	Super::DrawHUD();

	FVector2D ViewPortSize;
	if (GEngine) {
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
		const FVector2D ViewportCenter(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter) {
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairLeft) {
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairRight) {
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairTop) {
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, FVector2D(0, -SpreadScaled), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairBottom) {
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, FVector2D(0, SpreadScaled), HUDPackage.CrosshairsColor);
		}
	}
}

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor color)
{
	const float TextWidth = Texture->GetSizeX();
	const float TextHeight = Texture->GetSizeY();
	const FVector2D TextDrawPoint(ViewportCenter.X - (TextWidth / 2.f) + Spread.X, ViewportCenter.Y - (TextHeight / 2.f) + Spread.Y);

	DrawTexture(Texture, TextDrawPoint.X, TextDrawPoint.Y, TextWidth, TextHeight, 0.f, 0.f, 1.f, 1.f, color);
}