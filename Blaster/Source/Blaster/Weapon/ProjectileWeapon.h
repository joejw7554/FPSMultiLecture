
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

UCLASS()
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()


public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	void SetFireTrue();

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleSocket = "MuzzleFlash";

	UPROPERTY(EditDefaultsOnly)
	float FireDelay = 0.25f;

	UPROPERTY(VisibleAnywhere)
	bool bCanFire = true;

};
