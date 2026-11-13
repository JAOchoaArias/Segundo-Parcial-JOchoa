#include "Framework/PropHuntPlayerState.h"
#include "Net/UnrealNetwork.h"

APropHuntPlayerState::APropHuntPlayerState()
{
	bReplicates = true;
}

void APropHuntPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APropHuntPlayerState, PlayerRole);
	DOREPLIFETIME(APropHuntPlayerState, bIsAlive);
	DOREPLIFETIME(APropHuntPlayerState, Kills);
	DOREPLIFETIME(APropHuntPlayerState, Deaths);
}

void APropHuntPlayerState::SetPlayerRole(EPlayerRole NewRole)
{
	if (HasAuthority())
	{
		PlayerRole = NewRole;
	}
}

void APropHuntPlayerState::SetAliveStatus(bool bNewAlive)
{
	if (HasAuthority())
	{
		bIsAlive = bNewAlive;
	}
}

void APropHuntPlayerState::AddKill(float PointsToAdd)
{
	if (HasAuthority())
	{
		Kills++;
		SetScore(GetScore() + PointsToAdd); 
	}
}

void APropHuntPlayerState::AddDeath()
{
	if (HasAuthority())
	{
		Deaths++;
	}
}