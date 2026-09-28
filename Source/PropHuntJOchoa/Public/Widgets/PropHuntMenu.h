// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PropHuntMenu.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

UCLASS()
class PROPHUNTJOCHOA_API UPropHuntMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void UpdateRoleCounts(int32 HuntersCount, int32 PropsCount);
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_PlayHunter;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_PlayProp;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_StartGame;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_HuntersList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_PropsList;
	
private:
	UFUNCTION()
	void OnHunterClicked();
	
	UFUNCTION()
	void OnPropClicked();
	
	UFUNCTION()
	void OnStartGameClicked();
};