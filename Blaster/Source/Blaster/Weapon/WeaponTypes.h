#pragma once

#define TRACE_LENGTH 800000


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName="Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "SubMachine Gun"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),


	EWT_MAX UMETA(DisplayNames = "MAX"),

};