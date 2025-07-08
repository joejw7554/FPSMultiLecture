#include "Pickup.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"



APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));
	PickupMesh->bRenderCustomDepth = true;
	PickupMesh->SetCustomDepthStencilValue(250.f);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
	}

}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}


void APickup::Destroyed()
{
	Super::Destroyed();

	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation(), GetActorRotation());
	}
}

