#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PropHuntHUD.generated.h"

class UTextBlock;
class UProgressBar;
class UHealthComponent;

UCLASS()
class PROPHUNTJOCHOA_API UPropHuntHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void TriggerCrosshairFeedback(bool bHit);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

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
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Img_Crosshair;

private:
	FTimerHandle BindPawnTimerHandle;
	FTimerHandle CrosshairResetTimerHandle;

	UFUNCTION()
	void UpdateTimerText(int32 NewTime);

	UFUNCTION()
	void UpdateHealthUI(UHealthComponent* HealthComp, float CurrentHealth, float MaxHealth);

	void BindToLocalPawn();
	void ResetCrosshairColor();
	
	UFUNCTION()
	void UpdateTeamCountsUI(int32 Hunters, int32 Props);
};