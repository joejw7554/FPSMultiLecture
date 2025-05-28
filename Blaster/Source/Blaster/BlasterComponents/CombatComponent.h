#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS()
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ABlasterCharacter;

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
protected:
	virtual void BeginPlay() override;

private:
	class ABlasterCharacter* Character;

	AWeapon* EquippedWeapon;
};
