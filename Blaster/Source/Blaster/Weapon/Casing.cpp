#include "Casing.h"
#include "Components/StaticMeshComponent.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>("CasingMesh");
	SetRootComponent(CasingMesh);

}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

