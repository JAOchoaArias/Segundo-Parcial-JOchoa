// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/PropHuntGameState.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Widgets/PropHuntMenu.h" 
#include "PropHuntJOchoaCharacter.h"

APropHuntGameState::APropHuntGameState()
{
	bReplicates = true;
}

void APropHuntGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APropHuntGameState, HuntersCount);
	DOREPLIFETIME(APropHuntGameState, PropsCount);
}

void APropHuntGameState::RecalculateTeams()
{
	if (!HasAuthority()) return;

	int32 NewHunters = 0;
	int32 NewProps = 0;
	
	for (TActorIterator<APropHuntJOchoaCharacter> It(GetWorld()); It; ++It)
	{
		if (It->CurrentRole == EPlayerRole::Hunter)
		{
			NewHunters++;
		}
		else if (It->CurrentRole == EPlayerRole::Prop)
		{
			NewProps++;
		}
	}
	
	HuntersCount = NewHunters;
	PropsCount = NewProps;
	
	OnRep_TeamCounts();
}

void APropHuntGameState::OnRep_TeamCounts()
{
	for (TObjectIterator<UPropHuntMenu> It; It; ++It)
	{
		if (It->GetWorld() == GetWorld())
		{
			It->UpdateRoleCounts(HuntersCount, PropsCount);
		}
	}
}
