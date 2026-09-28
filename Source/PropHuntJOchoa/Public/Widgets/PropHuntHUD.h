// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PropHuntHUD.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PROPHUNTJOCHOA_API UPropHuntHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateHealth(float CurrentHealth, float MaxHealth);
	void UpdateRole(const FString& RoleName);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoleText;
};
