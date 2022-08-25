#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	EWT_AssaultRifle UMETA(DisplayName = "Assault Type"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),

	EWS_MAX UMETA(DisplayName = "Default Max")
};