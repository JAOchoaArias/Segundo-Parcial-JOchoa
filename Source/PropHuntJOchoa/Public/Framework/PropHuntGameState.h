#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Characters/PropHuntJOchoaCharacter.h"
#include "PropHuntGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, int32, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchStateChanged, EMatchState, NewState, EPlayerRole, Winner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamCountsChanged, int32, Hunters, int32, Props);

UCLASS()
class PROPHUNTJOCHOA_API APropHuntGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	APropHuntGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing = OnRep_TeamCounts, BlueprintReadOnly, Category = "Prop Hunt|Teams")
	int32 HuntersCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_TeamCounts, BlueprintReadOnly, Category = "Prop Hunt|Teams")
	int32 PropsCount = 0;

	void RecalculateTeams();
	
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "Prop Hunt|Match")
	int32 RemainingTime = 180; 

	UPROPERTY(ReplicatedUsing = OnRep_MatchState, BlueprintReadOnly, Category = "Prop Hunt|Match")
	EMatchState CurrentMatchState = EMatchState::Lobby;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Prop Hunt|Match")
	EPlayerRole WinningTeam = EPlayerRole::Unassigned;
	
	UPROPERTY(BlueprintAssignable, Category = "Prop Hunt|Events")
	FOnRemainingTimeChanged OnRemainingTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Prop Hunt|Events")
	FOnMatchStateChanged OnMatchStateChanged;
	
	void SetRemainingTime(int32 NewTime);
	void SetMatchState(EMatchState NewState, EPlayerRole Winner = EPlayerRole::Unassigned);
	
	UPROPERTY(BlueprintAssignable, Category = "Prop Hunt|Events")
	FOnTeamCountsChanged OnTeamCountsChanged;

protected:
	UFUNCTION()
	void OnRep_TeamCounts();

	UFUNCTION()
	void OnRep_RemainingTime();

	UFUNCTION()
	void OnRep_MatchState();
};