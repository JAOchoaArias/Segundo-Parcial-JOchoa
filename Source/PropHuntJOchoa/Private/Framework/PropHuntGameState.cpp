#include "Framework/PropHuntGameState.h"
#include "Framework/PropHuntPlayerState.h"
#include "Framework/PropHuntPlayerController.h"
#include "Net/UnrealNetwork.h"

APropHuntGameState::APropHuntGameState()
{
	bReplicates = true;
}

void APropHuntGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APropHuntGameState, HuntersCount);
	DOREPLIFETIME(APropHuntGameState, PropsCount);
	DOREPLIFETIME(APropHuntGameState, RemainingTime);
	DOREPLIFETIME(APropHuntGameState, CurrentMatchState);
	DOREPLIFETIME(APropHuntGameState, WinningTeam);
}

void APropHuntGameState::RecalculateTeams()
{
	if (!HasAuthority()) return;

	int32 TempHunters = 0;
	int32 TempProps = 0;

	for (APlayerState* PS : PlayerArray)
	{
		if (APropHuntPlayerState* PH_PS = Cast<APropHuntPlayerState>(PS))
		{
			if (PH_PS->PlayerRole == EPlayerRole::Hunter) TempHunters++;
			else if (PH_PS->PlayerRole == EPlayerRole::Prop) TempProps++;
		}
	}

	HuntersCount = TempHunters;
	PropsCount = TempProps;
	OnRep_TeamCounts();
}

void APropHuntGameState::SetRemainingTime(int32 NewTime)
{
	if (HasAuthority())
	{
		RemainingTime = FMath::Max(0, NewTime);
		OnRep_RemainingTime();
	}
}

void APropHuntGameState::SetMatchState(EMatchState NewState, EPlayerRole Winner)
{
	if (HasAuthority())
	{
		CurrentMatchState = NewState;
		WinningTeam = Winner;
		OnRep_MatchState();
	}
}

void APropHuntGameState::OnRep_TeamCounts()
{
	if (APropHuntPlayerController* PC = Cast<APropHuntPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->UpdateLobbyUI(HuntersCount, PropsCount);
	}
}

void APropHuntGameState::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void APropHuntGameState::OnRep_MatchState()
{
	OnMatchStateChanged.Broadcast(CurrentMatchState, WinningTeam);
}