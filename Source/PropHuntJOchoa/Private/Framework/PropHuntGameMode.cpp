#include "Framework/PropHuntGameMode.h"
#include "Framework/PropHuntGameState.h"
#include "Framework/PropHuntPlayerState.h"
#include "Framework/PropHuntPlayerController.h"
#include "TimerManager.h"
#include "Engine/World.h"

APropHuntGameMode::APropHuntGameMode()
{
	GameStateClass = APropHuntGameState::StaticClass();
	PlayerStateClass = APropHuntPlayerState::StaticClass();
	PlayerControllerClass = APropHuntPlayerController::StaticClass();
}

void APropHuntGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	StartMatchRound();
}

void APropHuntGameMode::StartMatchRound()
{
	if (APropHuntGameState* GS = GetGameState<APropHuntGameState>())
	{
		if (HidingDurationSeconds > 0)
		{
			GS->SetRemainingTime(HidingDurationSeconds);
			GS->SetMatchState(EMatchState::Hiding);
		}
		else
		{
			GS->SetRemainingTime(MatchDurationSeconds);
			GS->SetMatchState(EMatchState::InProgress);
		}
	}

	GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &APropHuntGameMode::OnRoundTimerTick, 1.0f, true);
}

void APropHuntGameMode::OnRoundTimerTick()
{
	APropHuntGameState* GS = GetGameState<APropHuntGameState>();
	if (!GS) return;

	if (GS->CurrentMatchState == EMatchState::Hiding)
	{
		const int32 NewTime = GS->RemainingTime - 1;
		GS->SetRemainingTime(NewTime);
		
		if (NewTime <= 0)
		{
			GS->SetRemainingTime(MatchDurationSeconds);
			GS->SetMatchState(EMatchState::InProgress);
		}
	}
	else if (GS->CurrentMatchState == EMatchState::InProgress)
	{
		const int32 NewTime = GS->RemainingTime - 1;
		GS->SetRemainingTime(NewTime);

		if (NewTime <= 0)
		{
			EndMatchWithWinner(EPlayerRole::Prop);
		}
	}
}

void APropHuntGameMode::OnPlayerEliminated(AController* VictimController, AController* KillerController)
{
	if (!VictimController) return;
	
	if (APropHuntPlayerState* VictimPS = VictimController->GetPlayerState<APropHuntPlayerState>())
	{
		VictimPS->SetAliveStatus(false);
		VictimPS->AddDeath();
	}
	
	if (APropHuntGameState* GS = GetGameState<APropHuntGameState>())
	{
		GS->RecalculateTeams();
	}
	
	if (KillerController && KillerController != VictimController)
	{
		if (APropHuntPlayerState* KillerPS = KillerController->GetPlayerState<APropHuntPlayerState>())
		{
			KillerPS->AddKill(100.0f);
		}
	}
	
	CheckWinConditions();
}

void APropHuntGameMode::CheckWinConditions()
{
	APropHuntGameState* GS = GetGameState<APropHuntGameState>();
	if (!GS || (GS->CurrentMatchState != EMatchState::InProgress && GS->CurrentMatchState != EMatchState::Hiding))
	{
		return;
	}

	int32 AliveProps = 0;
	int32 AliveHunters = 0;

	for (APlayerState* PS : GS->PlayerArray)
	{
		if (APropHuntPlayerState* PH_PS = Cast<APropHuntPlayerState>(PS))
		{
			if (PH_PS->bIsAlive)
			{
				if (PH_PS->PlayerRole == EPlayerRole::Prop) AliveProps++;
				else if (PH_PS->PlayerRole == EPlayerRole::Hunter) AliveHunters++;
			}
		}
	}

	if (GS->PlayerArray.Num() > 1)
	{
		if (AliveProps == 0)
		{
			EndMatchWithWinner(EPlayerRole::Hunter);
		}
		else if (AliveHunters == 0)
		{
			EndMatchWithWinner(EPlayerRole::Prop);
		}
	}
}

void APropHuntGameMode::EndMatchWithWinner(EPlayerRole WinningRole)
{
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);

	if (APropHuntGameState* GS = GetGameState<APropHuntGameState>())
	{
		GS->SetMatchState(EMatchState::MatchEnded, WinningRole);
	}
	
}