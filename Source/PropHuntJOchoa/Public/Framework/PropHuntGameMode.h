#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/PropHuntJOchoaCharacter.h"
#include "PropHuntGameMode.generated.h"

UCLASS()
class PROPHUNTJOCHOA_API APropHuntGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APropHuntGameMode();

	virtual void BeginPlay() override;
	
	void OnPlayerEliminated(AController* VictimController, AController* KillerController);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Rules")
	int32 MatchDurationSeconds = 180;

	FTimerHandle RoundTimerHandle;

	void StartMatchRound();
	void OnRoundTimerTick();
	void CheckWinConditions();
	void EndMatchWithWinner(EPlayerRole WinningRole);
};