#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/PropHuntTypes.h"
#include "EndMatchWidget.generated.h"

class UTextBlock;
class UButton;
class USoundBase;

UCLASS()
class PROPHUNTJOCHOA_API UEndMatchWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_ResultTitle;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_WinnerTeam;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_LoserTeam;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_PersonalResult;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_Scoreboard;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_ReturnMenu;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_ReturnLobby;
	
	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> VictorySound;

	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> DefeatSound;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Prop Hunt|UI")
	void OnMatchResultDetermined(EPlayerRole WinningTeam, EPlayerRole LosingTeam, bool bLocalPlayerWon);

private:
	UFUNCTION()
	void OnReturnMenuClicked();

	UFUNCTION()
	void OnReturnLobbyClicked();
};