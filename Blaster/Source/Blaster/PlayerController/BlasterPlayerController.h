#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	
private:
	class ABlasterHUD* BlasterHUD;
};
