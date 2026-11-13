#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Core/PropHuntTypes.h"
#include "PropHuntPlayerState.generated.h"

UCLASS()
class PROPHUNTJOCHOA_API APropHuntPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APropHuntPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Prop Hunt|PlayerState")
	EPlayerRole PlayerRole = EPlayerRole::Unassigned;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Prop Hunt|PlayerState")
	bool bIsAlive = true;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Prop Hunt|PlayerState")
	int32 Kills = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Prop Hunt|PlayerState")
	int32 Deaths = 0;
	
	void SetPlayerRole(EPlayerRole NewRole);
	void SetAliveStatus(bool bNewAlive);
	void AddKill(float PointsToAdd = 100.0f);
	void AddDeath();
};