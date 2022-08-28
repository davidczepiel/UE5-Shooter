// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/HUD/Anouncement.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/Image.h"

void ABlasterPlayerController::BeginPlay() {
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}

void ABlasterPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	//The time displayed in the HUD is fetched
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();

	CheckPing(DeltaTime);
}

void ABlasterPlayerController::CheckPing(float DeltaTime) {
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency) {
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState) {
			if (PlayerState->GetPing() * 4 > HighPingThreshold) {
				HighPingWarning();
				PingAnimationRunningTime = 0;
			}
			HighPingRunningTime = 0;
		}
	}
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation)) {
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration) {
			StopHighPingWarning();
		}
	}
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr) {
		if (BlasterHUD && BlasterHUD->CharacterOverlay) {
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay) {
				if (bInitHealth)SetHUDHelth(HUDHealth, HUDMaxHealth);
				if (bInitShield)SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitScore)SetHUDScore(HUDScore);
				if (bInitDefeats)SetHUDDefeats(HUDDefeats);
				if (bInitWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (bInitCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
			}
		}
	}
}

//////////////////////////////////Time sync server/client/////////////////////////////////////////////////

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	//Only controlled characters should ask for the time (they are the only ones with a HUD to display its value)
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency) {
		TimeSyncRunningTime = 0.f;
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTime);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

/////////////////////////////////////////////////Player respawn///////////////////////////////////////////////////////////

void ABlasterPlayerController::OnPossess(APawn* pawn) {
	Super::OnPossess(pawn);

	//If the controlled actor is a blaster character the HUD is updated
	ABlasterCharacter* character = Cast<ABlasterCharacter>(pawn);
	if (character) {
		SetHUDHelth(character->GetHealth(), character->GetMaxHealth());
	}
}

void ABlasterPlayerController::ReceivedPlayer() {
	Super::ReceivedPlayer();
	if (IsLocalController()) {
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

////////////////////////////////////////////////////////////////////////HUD//////////////////////////////////////////////////

void ABlasterPlayerController::SetHUDHelth(float Health, float MaxHealth) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar && BlasterHUD->CharacterOverlay->HealthText) {
		float percent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(percent);
		FString t = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(t));
	}
	//If HUD cannot be accesses or is not avaliable
	else {
		bInitHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ShieldBar && BlasterHUD->CharacterOverlay->ShieldText) {
		float percent = Shield / MaxShield;
		BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(percent);
		FString t = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(t));
	}
	//If HUD cannot be accesses or is not avaliable
	else {
		bInitShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

void ABlasterPlayerController::SetHUDScore(float Score) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount) {
		FString t = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(t));
	}
	//If HUD cannot be accesses or is not avaliable
	else {
		bInitScore = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount) {
		FString t = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(t));
	}
	//If HUD cannot be accesses or is not avaliable
	else {
		bInitDefeats = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 ammo) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount) {
		FString t = FString::Printf(TEXT("%d"), ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(t));
	}
	else {
		bInitWeaponAmmo = true;
		HUDWeaponAmmo = ammo;
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 ammo) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount) {
		FString t = FString::Printf(TEXT("%d"), ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(t));
	}
	else {
		bInitCarriedAmmo = true;
		HUDCarriedAmmo = ammo;
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float time) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountDownText) {
		if (time < 0.f) {
			BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText());
			return;
		}
		int32 min = FMath::FloorToInt(time / 60.f);
		int32 sec = time - min * 60;

		FString t = FString::Printf(TEXT("%02d : %02d"), min, sec);
		BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(t));
	}
}

void ABlasterPlayerController::SetHUDAnouncementCountDown(float time) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->Anouncement &&
		BlasterHUD->Anouncement->WarmUpTime) {
		if (time < 0.f) {
			BlasterHUD->Anouncement->WarmUpTime->SetText(FText());
			return;
		}

		int32 min = FMath::FloorToInt(time / 60.f);
		int32 sec = time - min * 60;

		FString t = FString::Printf(TEXT("%02d : %02d"), min, sec);
		BlasterHUD->Anouncement->WarmUpTime->SetText(FText::FromString(t));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmUptime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmUptime + matchtime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = WarmUptime + matchtime - GetServerTime() + LevelStartingTime + CooldownTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority()) {
		GameMode = GameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : GameMode;
		if (GameMode) {
			SecondsLeft = FMath::CeilToInt(GameMode->GetCountdownTime() + LevelStartingTime);
		}
	}
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnouncementCountDown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

/////////////////////////////////////////////////Match stuff/////////////////////////////////////////////////////////////////

void ABlasterPlayerController::OnMatchStateSet(FName state) {
	MatchState = state;

	//if (MatchState == MatchState::WaitingToStart) {
	//	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	//	if (BlasterHUD) {
	//		BlasterHUD->AddCharacterOverlay();
	//	}
	//}

	if (MatchState == MatchState::InProgress) {
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown) {
		HandleCooldown();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* g = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (g) {
		WarmUptime = g->WarmupTime;
		matchtime = g->MatchTime;
		CooldownTime = g->CooldownTime;
		LevelStartingTime = g->LevelStartingTime;
		MatchState = g->GetMatchState();
		ClientJoinMidGame(MatchState, WarmUptime, matchtime, LevelStartingTime, CooldownTime);
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName state, float warmup, float match, float starttime, float cooldown)
{
	WarmUptime = warmup;
	matchtime = match;
	LevelStartingTime = starttime;
	MatchState = state;
	OnMatchStateSet(MatchState);
	if (BlasterHUD && MatchState == MatchState::WaitingToStart) {
		BlasterHUD->AddAnouncement();
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation && BlasterHUD->CharacterOverlay->HighPingImage) {
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation && BlasterHUD->CharacterOverlay->HighPingImage) {
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress) {
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown) {
		HandleCooldown();
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD) {
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		if (BlasterHUD->Anouncement &&
			BlasterHUD->Anouncement->AnouncementText &&
			BlasterHUD->Anouncement->InfoText) {
			BlasterHUD->Anouncement->SetVisibility(ESlateVisibility::Visible);
			FString a("New match starts in: ");
			BlasterHUD->Anouncement->AnouncementText->SetText(FText::FromString(a));
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				BlasterHUD->Anouncement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}

	ABlasterCharacter* c = Cast<ABlasterCharacter>(GetPawn());
	if (c && c->GetCombat()) {
		c->bDisableGameplay = true;
		c->GetCombat()->FireButtonPressed(false);
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD) {
		if (BlasterHUD->CharacterOverlay == nullptr)BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Anouncement) {
			BlasterHUD->Anouncement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}