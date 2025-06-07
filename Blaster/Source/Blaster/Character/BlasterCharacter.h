#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);

protected:
	virtual void BeginPlay() override;

	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpButtonPressed(const FInputActionValue& Value);


	void EquipButtonPressed(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	void CrouchButtonPressed(const FInputActionValue& Value);

	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);

	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);


	void AimOffset(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true") )
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon, VisibleAnywhere)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	//////////////////Input//////////////////////////
	UPROPERTY(EditDefaultsOnly, Category= Input)
	class UInputMappingContext* BlasterCharacterContext;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	 UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	 UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	 UInputAction* EquipAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* FireAction;

	////////////////////Components
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	UPROPERTY(Replicated)
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category=Combat)
	class UAnimMontage* FireWeaponMontage;

	void HideCameraIfCharacterClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	float GetAO_Yaw() const { return AO_Yaw; }
	float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	ETurningInPlace GetTurningInplace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
