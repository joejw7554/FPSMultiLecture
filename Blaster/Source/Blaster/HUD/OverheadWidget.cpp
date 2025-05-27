#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority: Role = FString("Autority"); break;
	case ENetRole::ROLE_AutonomousProxy: Role = FString("Autonomous Proxy");  break;
	case ENetRole::ROLE_SimulatedProxy: Role = FString("Simulated Proxy"); break;
	case ENetRole::ROLE_None: Role = FString("None"); break;
	}

	FString RemoteRoleRoleString = FString::Printf(TEXT("RemoteRole Role: %s"), *Role);

	SetDisplayText(RemoteRoleRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();

}
