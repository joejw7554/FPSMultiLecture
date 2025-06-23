#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayNames = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayNames = "Rocket Launcher"),
	EWT_MAX UMETA(DisplayNames = "MAX"),

};