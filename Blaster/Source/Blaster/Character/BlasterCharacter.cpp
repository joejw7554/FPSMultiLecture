#include "BlasterCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"


#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon//WeaponTypes.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/GameMode/BlasterGameMode.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->SetRelativeLocation(FVector(0, 0, 90.f));
	CameraBoom->SocketOffset = FVector(0, 75.f, 75.f);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->PostProcessSettings.DepthOfFieldFocalDistance = 1e4;
	FollowCamera->PostProcessSettings.DepthOfFieldFstop = 32.f;


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(55.f);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);


	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(RootComponent);
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidget->SetDrawAtDesiredSize(true);

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);


	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>("DissolveTimelineComponent");

	//CDO
	ConstructorHelpers::FObjectFinder<USkeletalMesh>mesh(L"/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'");
	if (mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(mesh.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0.f));

	//Check this frequently
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);


	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	MinNetUpdateFrequency = 33.f;
	NetUpdateFrequency = 66.f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInPorgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;

	if (Combat && Combat->EquippedWeapon && bMatchNotInPorgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;

}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

	/////////////Enhanced Input Process
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (InputSubsystem)
		{
			InputSubsystem->AddMappingContext(BlasterCharacterContext, 0);
		}
	}
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (InputSubsystem)
		{
			InputSubsystem->AddMappingContext(BlasterCharacterContext, 0);
		}
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}

}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();

		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::Movement(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	// Get the input vector from the input action value  
	const FVector2D InputVector = Value.Get<FVector2D>();

	// Calculate the forward and right movement directions based on the character's control rotation  
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Add movement input in the forward and right directions  
	AddMovementInput(ForwardDirection, InputVector.Y);
	AddMovementInput(RightDirection, InputVector.X);
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();

	AddControllerYawInput(InputVector.X);
	AddControllerPitchInput(InputVector.Y);
}



void ABlasterCharacter::JumpButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Jump();
	}
}

void ABlasterCharacter::EquipButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	const bool bPressed = Value.Get<bool>();

	if (bPressed)
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}
	}
}

void ABlasterCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	const bool bPressed = Value.Get<bool>();
	if (Combat)
	{
		Combat->FireButtonPressed(bPressed);
	}
}

void ABlasterCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;


	if (Combat)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;


	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) //stop
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //Runing or Jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//map pitch from [270, 360) to [-90,0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;


	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;

		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}

}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABlasterCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}


void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (!DynamicDissolveMaterialInstance.IsEmpty())
	{
		for (auto Instance : DynamicDissolveMaterialInstance)
		{
			Instance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
		}
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);

	if (DissovleCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissovleCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();

	return Combat->HitTarget;
}

ECombateState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombateState::ECS_MAX;

	return Combat->CombatState;
}


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}
void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();

	}
}
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		if (MoveAction)
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Movement);

		if (LookAction)
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);

		if (JumpAction)
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::JumpButtonPressed);

		if (EquipAction)
			EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::EquipButtonPressed);

		if (CrouchAction)
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::CrouchButtonPressed);

		if (AimAction)
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);

		if (AimAction)
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);

		if (FireAction)
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::FireButtonPressed);

		if (FireAction)
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);

		if (ReloadAction)
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &ABlasterCharacter::ReloadButtonPressed);
	}

}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);

}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	if (FireWeaponMontage)
	{
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		PlayAnimMontage(FireWeaponMontage, 1.f, SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	if (ReloadMontage)
	{
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;

		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rifle"); //Need to be fixed later
		}

		PlayAnimMontage(ReloadMontage, 1.f, SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	if (ElimMontage)
	{
		PlayAnimMontage(ElimMontage);
	}
}

void ABlasterCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}

	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
}

void ABlasterCharacter::MulticastElim_Implementation()
{

	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}

	bElimmed = true;
	PlayElimMontage();

	//Start Dissovle Effect
	if (DissolveMaterialInstanceOne && DissolveMaterialInstanceTwo)
	{
		DynamicDissolveMaterialInstance.Add(UMaterialInstanceDynamic::Create(DissolveMaterialInstanceOne, this));
		DynamicDissolveMaterialInstance.Add(UMaterialInstanceDynamic::Create(DissolveMaterialInstanceTwo, this));


		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance[0]);
		GetMesh()->SetMaterial(1, DynamicDissolveMaterialInstance[1]);

		for (auto Instance : DynamicDissolveMaterialInstance)
		{
			Instance->SetScalarParameterValue(TEXT("Dissolve"), -0.55f);
			Instance->SetScalarParameterValue(TEXT("Glow"), 200.f);
		}
	}
	StartDissolve();

	//Disable Character Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	//Disable Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//Spawn Elim Bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimBotEffect, ElimBotSpawnPoint, GetActorRotation());
	}

	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ElimBotSound, GetActorLocation());
	}

}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();

	if (BlasterGameMode)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}


}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	{
		FName SectionName = "FromFront";
		PlayAnimMontage(HitReactMontage, 1.f, SectionName);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	UpdateHUDHealth();
	PlayHitReactMontage();


	if (Health <= 0.f)
	{
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatedBy);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}




