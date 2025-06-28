#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

#include "Blaster/Character/BlasterCharacter.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstingatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MeshFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MeshFlashSocket && InstingatorController)
	{
		FTransform SocketTransform = MeshFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25f;
		FHitResult FireHit;

		UWorld* World= GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);

			if (FireHit.bBlockingHit)
			{
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
				if (BlasterCharacter)
				{
					if(HasAuthority())
					{
						UGameplayStatics::ApplyDamage(BlasterCharacter, Damage, InstingatorController, this, UDamageType::StaticClass());
					}
				}

				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
				}
			}
		}
	}

}
