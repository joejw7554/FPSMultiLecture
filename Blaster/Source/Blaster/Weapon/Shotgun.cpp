#include "Shotgun.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "Blaster/Character/BlasterCharacter.h"



void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstingatorController = OwnerPawn->GetController();


	const USkeletalMeshSocket* MeshFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MeshFlashSocket)
	{
		FTransform SocketTransform = MeshFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			FVector End = TraceEndWithScatter(Start, HitTarget);
		}

	}

}
