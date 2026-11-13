#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PropHuntHUD.generated.h"

class UTextBlock;
class UProgressBar;
class UHealthComponent;

UCLASS()
class PROPHUNTJOCHOA_API UPropHuntHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_Timer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_Health;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> Bar_Health;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_Role;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_PropsAlive;

private:
	UFUNCTION()
	void UpdateTimerText(int32 NewTime);

	UFUNCTION()
	void UpdateHealthUI(UHealthComponent* HealthComp, float CurrentHealth, float MaxHealth);
};