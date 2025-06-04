#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	ACasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CasingMesh;

private:
	UPROPERTY(EditDefaultsOnly)
	float ShellEjectionImpulse = 10.f;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* ShellSound;


};
