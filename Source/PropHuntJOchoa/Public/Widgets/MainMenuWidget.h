#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UEditableTextBox;
class UWidgetSwitcher; 

UCLASS()
class PROPHUNTJOCHOA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MenuSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Play;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Quit;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_GoToMultiplayer; 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_GoToLAN; 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_BackToStart; 
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_HostLAN;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_JoinLAN;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_BackToModes; 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> Txt_IPInput; 

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PluginMenuClass;

private:
	UFUNCTION()
	void OnPlayClicked();

	UFUNCTION()
	void OnGoToLANClicked();

	UFUNCTION()
	void OnBackToStartClicked();

	UFUNCTION()
	void OnBackToModesClicked();
	
	UFUNCTION()
	void OnGoToMultiplayerClicked();

	UFUNCTION()
	void OnHostLANClicked();

	UFUNCTION()
	void OnJoinLANClicked();

	UFUNCTION()
	void OnQuitClicked();
};