// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PropHuntGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROPHUNTJOCHOA_API APropHuntGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	APropHuntGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void RecalculateTeams();
	
protected:
	
	UPROPERTY(ReplicatedUsing = OnRep_TeamCounts)
	int32 HuntersCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_TeamCounts)
	int32 PropsCount = 0;
	
	UFUNCTION()
	void OnRep_TeamCounts();
};
