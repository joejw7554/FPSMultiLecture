#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WeaponTypes.h"

#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_Max UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void SetHUDAmmo();
	void AddAmmo(int32 AmmoToAdd);

	////////////////////////Textures for the weapon crosshairs
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	//Zoomed FOV while Aiming//////////////////////////////////////
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 45.f;

	UPROPERTY(EditAnywhere)
	float ZoomedInterpSpeed = 30.f;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;


	//Enable or disable custom depth
	void EnableCustomDepth(bool bEnable);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo = 30;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();


	UPROPERTY(EditAnywhere)
	int32 MagCapacity = 30;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere)
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;


public:
	void SetWeaponState(EWeaponState State);
	USphereComponent* GetAreaSphere() const { return AreaSphere; }
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	float GetZoomedFOV() const { return ZoomedFOV; }
	float GetZoomedInterpSpeed() const { return ZoomedInterpSpeed; }
	bool IsEmpty();
	bool IsFull();

	EWeaponType  GetWeaponType() const { return WeaponType; }
	int32 GetAmmo()const { return Ammo; }
	int32 GetMagCapacity()const { return MagCapacity; }
};
