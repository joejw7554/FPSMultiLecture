#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;


private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;
};
