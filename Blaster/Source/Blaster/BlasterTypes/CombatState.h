#pragma once


UENUM(BlueprintType)
enum class ECombateState : uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_ThrowingGrenade UMETA(DisplayName = "Throwing Grenade"),


	ECS_MAX UMETA(DisplayName = "DefaultMax"),

};