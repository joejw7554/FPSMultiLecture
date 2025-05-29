
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

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




	////////////////////Components
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;


public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
};
