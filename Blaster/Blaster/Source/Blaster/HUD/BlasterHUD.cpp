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

		//The crosshair spread is calculated from the HUDPackage available
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		//Each part of the crosshair is drawed
		if (HUDPackage.CrosshairCenter)	DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, FVector2D(0.f, 0.f), HUDPackage.CrosshairsColor);
		if (HUDPackage.CrosshairLeft)	DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0), HUDPackage.CrosshairsColor);
		if (HUDPackage.CrosshairRight)	DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0), HUDPackage.CrosshairsColor);
		if (HUDPackage.CrosshairTop)	DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, FVector2D(0, -SpreadScaled), HUDPackage.CrosshairsColor);
		if (HUDPackage.CrosshairBottom)	DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, FVector2D(0, SpreadScaled), HUDPackage.CrosshairsColor);
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
		//A new killfeed element is created using the names of the players involved in the kill
		UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayer, ElimAnnouncementClass);
		if (ElimAnnouncementWidget)
		{
			ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();

			//All the existing elim anouncements are moved to make space for the new one
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

			//The new anouncement is added
			ElimMessages.Add(ElimAnnouncementWidget);

			//A timer is set to remove the anouncement after a short amount of time
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
	//THe elemtent is removed from the screen
	if (MsgToRemove)		MsgToRemove->RemoveFromParent();
}