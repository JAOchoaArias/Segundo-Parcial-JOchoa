// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PropHuntHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UPropHuntHUD::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.f)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UPropHuntHUD::UpdateRole(const FString& RoleName)
{
	if (RoleText)
	{
		RoleText->SetText(FText::FromString(RoleName));
	}
}
