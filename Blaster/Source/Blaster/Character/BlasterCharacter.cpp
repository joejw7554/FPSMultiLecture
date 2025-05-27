#include "BlasterCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->SetRelativeLocation(FVector(0, 0, -22.f));
	CameraBoom->SocketOffset = FVector(0, 75.f, 75.f);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;


	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(RootComponent);
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidget->SetDrawAtDesiredSize(true);


	//CDO
	ConstructorHelpers::FObjectFinder<USkeletalMesh>mesh(L"/Script/Engine.SkeletalMesh'/Game/Mixamo/CharacterMaterials/Eve_By_J_Gonzales.Eve_By_J_Gonzales'");
	if (mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(mesh.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0.f));

}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

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

void ABlasterCharacter::Movement(const FInputActionValue& Value)  
{  
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

void ABlasterCharacter::Jump(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();

	if (bPressed)
	{
		ACharacter::Jump();
	}
}


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Jump);
	}

}




