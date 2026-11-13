#include "Framework/PropHuntPlayerController.h"
#include "Framework/PropHuntPlayerState.h"
#include "Characters/PropHuntJOchoaCharacter.h"
#include "UI/PropHuntMenu.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void APropHuntPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		InitializeMatchUI();
	}
}

void APropHuntPlayerController::InitializeMatchUI()
{
	APropHuntGameState* GS = GetWorld()->GetGameState<APropHuntGameState>();
	if (!GS)
	{
		GetWorldTimerManager().SetTimer(InitUITimerHandle, this, &APropHuntPlayerController::InitializeMatchUI, 0.2f, false);
		return;
	}

	GS->OnMatchStateChanged.AddDynamic(this, &APropHuntPlayerController::HandleMatchStateChanged);
	HandleMatchStateChanged(GS->CurrentMatchState, GS->WinningTeam);
}

void APropHuntPlayerController::HandleMatchStateChanged(EMatchState NewState, EPlayerRole Winner)
{
	if (!IsLocalController()) return;

	if (NewState == EMatchState::InProgress)
	{
		if (GameHUDClass && !ActiveHUDWidget)
		{
			ActiveHUDWidget = CreateWidget<UUserWidget>(this, GameHUDClass);
			if (ActiveHUDWidget)
			{
				ActiveHUDWidget->AddToViewport();
			}
		}
	}
	else if (NewState == EMatchState::MatchEnded)
	{
		if (ActiveHUDWidget)
		{
			ActiveHUDWidget->RemoveFromParent();
			ActiveHUDWidget = nullptr;
		}
		
		if (EndMatchWidgetClass && !ActiveEndMatchWidget)
		{
			ActiveEndMatchWidget = CreateWidget<UUserWidget>(this, EndMatchWidgetClass);
			if (ActiveEndMatchWidget)
			{
				ActiveEndMatchWidget->AddToViewport();

				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(ActiveEndMatchWidget->TakeWidget());
				SetInputMode(InputMode);
				bShowMouseCursor = true;
			}
		}
	}
}

void APropHuntPlayerController::Server_RequestRole_Implementation(EPlayerRole RequestedRole)
{
	if (APropHuntPlayerState* PH_PS = GetPlayerState<APropHuntPlayerState>())
	{
		PH_PS->SetPlayerRole(RequestedRole);
	}

	if (APropHuntJOchoaCharacter* MyCharacter = Cast<APropHuntJOchoaCharacter>(GetPawn()))
	{
		MyCharacter->SetRole(RequestedRole);
	}
	else if (APropHuntGameState* GS = GetWorld()->GetGameState<APropHuntGameState>())
	{
		GS->RecalculateTeams();
	}
}

void APropHuntPlayerController::Server_StartMatch_Implementation()
{
	if (HasAuthority())
	{
		GetWorld()->ServerTravel("/Game/Characters/ThirdPerson/Lvl_ThirdPerson?listen");
	}
}

void APropHuntPlayerController::Client_NotifyAttackResult_Implementation(bool bHitTarget)
{
	if (bHitTarget)
	{
		if (AttackHitSound) UGameplayStatics::PlaySound2D(this, AttackHitSound);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("¡IMPACTO! (-25 HP al Prop)"));
	}
	else
	{
		if (AttackMissSound) UGameplayStatics::PlaySound2D(this, AttackMissSound);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange, TEXT("FALLO (-5 HP de penalización)"));
	}
}

void APropHuntPlayerController::UpdateLobbyUI(int32 HuntersCount, int32 PropsCount)
{
	if (!IsLocalController()) return;

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UPropHuntMenu::StaticClass(), false);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (UPropHuntMenu* LobbyMenu = Cast<UPropHuntMenu>(Widget))
		{
			LobbyMenu->UpdateRoleCounts(HuntersCount, PropsCount);
		}
	}
}