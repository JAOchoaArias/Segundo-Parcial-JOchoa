#include "UI/PropHuntHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Framework/PropHuntGameState.h"
#include "Characters/PropHuntJOchoaCharacter.h"
#include "Gameplay/Components/HealthComponent.h"

void UPropHuntHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (APropHuntGameState* GS = GetWorld()->GetGameState<APropHuntGameState>())
	{
		GS->OnRemainingTimeChanged.AddDynamic(this, &UPropHuntHUDWidget::UpdateTimerText);
		UpdateTimerText(GS->RemainingTime);

		if (Txt_PropsAlive)
		{
			Txt_PropsAlive->SetText(FText::FromString(FString::Printf(TEXT("Props: %d"), GS->PropsCount)));
		}
	}
	
	if (APropHuntJOchoaCharacter* MyChar = Cast<APropHuntJOchoaCharacter>(GetOwningPlayerPawn()))
	{
		if (Txt_Role)
		{
			FString RoleStr = (MyChar->CurrentRole == EPlayerRole::Hunter) ? TEXT("CAZADOR") : TEXT("PROP");
			Txt_Role->SetText(FText::FromString(RoleStr));
		}

		if (UHealthComponent* HealthComp = MyChar->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->OnHealthChanged.AddDynamic(this, &UPropHuntHUDWidget::UpdateHealthUI);
			UpdateHealthUI(HealthComp, HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
		}
	}
}

void UPropHuntHUDWidget::UpdateTimerText(int32 NewTime)
{
	if (Txt_Timer)
	{
		const int32 Minutes = NewTime / 60;
		const int32 Seconds = NewTime % 60;
		Txt_Timer->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
	}
}

void UPropHuntHUDWidget::UpdateHealthUI(UHealthComponent* HealthComp, float CurrentHealth, float MaxHealth)
{
	if (Txt_Health)
	{
		Txt_Health->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth)));
	}
	if (Bar_Health && MaxHealth > 0.0f)
	{
		Bar_Health->SetPercent(CurrentHealth / MaxHealth);
	}
}