#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	/*if (!bCanFire) return;
	bCanFire = false;
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AProjectileWeapon::SetFireTrue, FireDelay, false, -1.f);*/

	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(MuzzleSocket);
	if (ProjectileClass && MuzzleFlashSocket && InstigatorPawn)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		FVector ToTarget = HitTarget - SocketTransform.GetLocation(); //FromMuzzle Flash Socket to hit location from TraceUnderCrossHairs
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
	}
}

void AProjectileWeapon::SetFireTrue()
{
	bCanFire = true;
	UE_LOG(LogTemp, Warning, TEXT("SetFireTrue Called"));
}
