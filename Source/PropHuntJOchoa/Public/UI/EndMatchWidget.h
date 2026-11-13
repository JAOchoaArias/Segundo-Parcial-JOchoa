#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndMatchWidget.generated.h"

class UTextBlock;
class UButton;

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
	TObjectPtr<UTextBlock> Txt_Scoreboard;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_ReturnMenu;

private:
	UFUNCTION()
	void OnReturnMenuClicked();
};