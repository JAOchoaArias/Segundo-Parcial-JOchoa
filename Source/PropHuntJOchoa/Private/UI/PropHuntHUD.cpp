#include "UI/PropHuntHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Framework/PropHuntGameState.h"
#include "Characters/PropHuntJOchoaCharacter.h"
#include "Gameplay/Components/HealthComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/Image.h"

void UPropHuntHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (APropHuntGameState* GS = GetWorld()->GetGameState<APropHuntGameState>())
	{
		GS->OnRemainingTimeChanged.AddDynamic(this, &UPropHuntHUDWidget::UpdateTimerText);
		GS->OnTeamCountsChanged.AddDynamic(this, &UPropHuntHUDWidget::UpdateTeamCountsUI);

		UpdateTimerText(GS->RemainingTime);
		UpdateTeamCountsUI(GS->HuntersCount, GS->PropsCount);
	}

	BindToLocalPawn();
}

void UPropHuntHUDWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BindPawnTimerHandle);
		World->GetTimerManager().ClearTimer(CrosshairResetTimerHandle);

		if (APropHuntGameState* GS = World->GetGameState<APropHuntGameState>())
		{
			GS->OnRemainingTimeChanged.RemoveDynamic(this, &UPropHuntHUDWidget::UpdateTimerText);
			GS->OnTeamCountsChanged.RemoveDynamic(this, &UPropHuntHUDWidget::UpdateTeamCountsUI);
		}
	}

	if (APropHuntJOchoaCharacter* MyChar = Cast<APropHuntJOchoaCharacter>(GetOwningPlayerPawn()))
	{
		if (UHealthComponent* HealthComp = MyChar->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->OnHealthChanged.RemoveDynamic(this, &UPropHuntHUDWidget::UpdateHealthUI);
		}
	}

	Super::NativeDestruct();
}

void UPropHuntHUDWidget::UpdateTimerText(int32 NewTime)
{
	if (Txt_Timer)
	{
		const int32 Minutes = NewTime / 60;
		const int32 Seconds = NewTime % 60;

		APropHuntGameState* GS = GetWorld() ? GetWorld()->GetGameState<APropHuntGameState>() : nullptr;
		const bool bIsHiding = (GS && GS->CurrentMatchState == EMatchState::Hiding);

		FString FormattedTime = bIsHiding
			? FString::Printf(TEXT("ESCONDITE: %02d:%02d"), Minutes, Seconds)
			: FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		Txt_Timer->SetText(FText::FromString(FormattedTime));
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

void UPropHuntHUDWidget::BindToLocalPawn()
{
	APropHuntJOchoaCharacter* MyChar = Cast<APropHuntJOchoaCharacter>(GetOwningPlayerPawn());
	UHealthComponent* HealthComp = MyChar ? MyChar->FindComponentByClass<UHealthComponent>() : nullptr;

	if (!MyChar || !HealthComp)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(BindPawnTimerHandle, this, &UPropHuntHUDWidget::BindToLocalPawn, 0.1f, false);
		}
		return;
	}
	
	const bool bIsHunter = (MyChar->CurrentRole == EPlayerRole::Hunter);
	
	if (Txt_Role)
	{
		FString RoleStr = (MyChar->CurrentRole == EPlayerRole::Hunter) ? TEXT("CAZADOR") : TEXT("PROP");
		Txt_Role->SetText(FText::FromString(RoleStr));
	}
	if (Img_Crosshair)
	{
		Img_Crosshair->SetVisibility(bIsHunter ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		Img_Crosshair->SetColorAndOpacity(FLinearColor::White);
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &UPropHuntHUDWidget::UpdateHealthUI);
	UpdateHealthUI(HealthComp, HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
}

void UPropHuntHUDWidget::UpdateTeamCountsUI(int32 Hunters, int32 Props)
{
	if (Txt_PropsAlive)
	{
		Txt_PropsAlive->SetText(FText::FromString(FString::Printf(TEXT("Props: %d"), Props)));
	}
}

void UPropHuntHUDWidget::TriggerCrosshairFeedback(bool bHit)
{
	if (!Img_Crosshair) return;

	Img_Crosshair->SetColorAndOpacity(bHit ? FLinearColor::Green : FLinearColor::Red);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CrosshairResetTimerHandle, this, &UPropHuntHUDWidget::ResetCrosshairColor, 0.25f, false);
	}
}

void UPropHuntHUDWidget::ResetCrosshairColor()
{
	if (Img_Crosshair)
	{
		Img_Crosshair->SetColorAndOpacity(FLinearColor::White);
	}
}