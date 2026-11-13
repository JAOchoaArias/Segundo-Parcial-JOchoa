#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/PropHuntTypes.h"
#include "Framework/PropHuntGameState.h"
#include "PropHuntPlayerController.generated.h"

class UUserWidget;
class USoundBase;

UCLASS()
class PROPHUNTJOCHOA_API APropHuntPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_RequestRole(EPlayerRole RequestedRole);

	UFUNCTION(Server, Reliable)
	void Server_StartMatch();

	UFUNCTION(Client, Reliable)
	void Client_NotifyAttackResult(bool bHitTarget);

	void UpdateLobbyUI(int32 HuntersCount, int32 PropsCount);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|UI")
	TSubclassOf<UUserWidget> GameHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|UI")
	TSubclassOf<UUserWidget> EndMatchWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> HitConfirmSound;

	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> MissPenaltySound;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveHUDWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveEndMatchWidget;

	FTimerHandle InitUITimerHandle;

	void InitializeMatchUI();

	UFUNCTION()
	void HandleMatchStateChanged(EMatchState NewState, EPlayerRole Winner);
};