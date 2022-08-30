// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Anouncement.h"
#include "ElimAnnouncement.h"

#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterHUD::DrawHUD() {
	Super::DrawHUD();

	FVector2D ViewPortSize;
	if (GEngine) {
		//Viewport dimensions
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
		const FVector2D ViewportCenter(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);

		//Crosshair spread
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		//Crosshair drawing
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

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	//If both the character and its overlay class are avaliable a new overlay is created and displayed
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::AddAnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	//If both the character and its anouncements overlay class are avaliable a new overlay is created and displayed
	if (PlayerController && AnouncementClass)
	{
		Anouncement = CreateWidget<UAnouncement>(PlayerController, AnouncementClass);
		Anouncement->AddToViewport();
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor color)
{
	const float TextWidth = Texture->GetSizeX();
	const float TextHeight = Texture->GetSizeY();

	//Position from where the texture needs to start its drawing
	const FVector2D TextDrawPoint(ViewportCenter.X - (TextWidth / 2.f) + Spread.X, ViewportCenter.Y - (TextHeight / 2.f) + Spread.Y);
	DrawTexture(Texture, TextDrawPoint.X, TextDrawPoint.Y, TextWidth, TextHeight, 0.f, 0.f, 1.f, 1.f, color);
}

void ABlasterHUD::AddElimAnnouncement(FString Attacker, FString Victim)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
	if (OwningPlayer && ElimAnnouncementClass)
	{
		UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayer, ElimAnnouncementClass);
		if (ElimAnnouncementWidget)
		{
			ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();

			for (UElimAnnouncement* Msg : ElimMessages)
			{
				if (Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if (CanvasSlot)
					{
						FVector2D Position = CanvasSlot->GetPosition();
						FVector2D NewPosition(
							CanvasSlot->GetPosition().X,
							Position.Y - CanvasSlot->GetSize().Y
						);
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}

			ElimMessages.Add(ElimAnnouncementWidget);

			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgDelegate;
			ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(
				ElimMsgTimer,
				ElimMsgDelegate,
				ElimAnnouncementTime,
				false
			);
		}
	}
}

void ABlasterHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
	if (MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
	}
}