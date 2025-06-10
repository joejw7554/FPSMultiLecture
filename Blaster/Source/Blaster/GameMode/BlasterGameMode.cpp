#include "BlasterGameMode.h"

#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Character/BlasterCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}

}

void ABlasterGameMode::RequestRespawn(ABlasterCharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}

}
