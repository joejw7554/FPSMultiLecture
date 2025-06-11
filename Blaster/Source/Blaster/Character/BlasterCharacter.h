#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"

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

	void PlayElimMontage();

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	virtual void Destroyed() override;

	virtual void OnRep_ReplicatedMovement() override;


protected:
	virtual void BeginPlay() override;

	void UpdateHUDHealth();

	//Poll for any relevant class and intialize our HUD
	void PollInit();

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
	void CalculateAO_Pitch();
	void SimProxiesTurn();

	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

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

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category=Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;


	void HideCameraIfCharacterClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();


	///////PlayerHealth////////////////////////////

	UPROPERTY(EditAnywhere, Category= "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats" )
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 5.f;

	void ElimTimerFinished();

	///Disolve Effect////////////////////////////

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissovleCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	//Dynamic Instance we can change at run time;
	UPROPERTY(VisibleAnywhere, Category=Elim)
	TArray<UMaterialInstanceDynamic*> DynamicDissolveMaterialInstance;

	//Material Instance set on the blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* DissolveMaterialInstanceOne;

	//Material Instance set on the blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstanceTwo;


	//Elim bot////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	class ABlasterPlayerState* BlasterPlayerState;

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
	bool ShouldRotateRootBone() const { return bRotateRootBone; }
	bool IsElimmed() const { return bElimmed; }
	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }

};
