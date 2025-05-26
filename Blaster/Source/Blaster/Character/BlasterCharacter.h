
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



protected:
	virtual void BeginPlay() override;

	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

private:


	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category= Input)
	class UInputMappingContext* BlasterCharacterContext;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* JumpAction;


};
