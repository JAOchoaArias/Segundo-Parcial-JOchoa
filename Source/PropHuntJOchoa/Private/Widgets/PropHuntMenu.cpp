// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/PropHuntMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Public/GameInstaces/PropHuntGameInstance.h"
#include "PlayerControllers/PropHuntPlayerController.h"


void UPropHuntMenu::UpdateRoleCounts(int32 HuntersCount, int32 PropsCount)
{
    if (Txt_HuntersList && Txt_PropsList)
    {
        Txt_HuntersList->SetText(FText::FromString(FString::Printf(TEXT("Cazadores: %d"), HuntersCount)));
        Txt_PropsList->SetText(FText::FromString(FString::Printf(TEXT("Props: %d"), PropsCount)));
    }
}

void UPropHuntMenu::NativeConstruct()
{
    Super::NativeConstruct();
	
    if (Btn_PlayHunter) Btn_PlayHunter->OnClicked.AddDynamic(this, &UPropHuntMenu::OnHunterClicked);
    if (Btn_PlayProp) Btn_PlayProp->OnClicked.AddDynamic(this, &UPropHuntMenu::OnPropClicked);
    if (Btn_StartGame) Btn_StartGame->OnClicked.AddDynamic(this, &UPropHuntMenu::OnStartGameClicked);
	
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (!PC->HasAuthority())
        {
            Btn_StartGame->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UPropHuntMenu::OnHunterClicked()
{
    if (APropHuntPlayerController* PC = Cast<APropHuntPlayerController>(GetOwningPlayer()))
    {
        if (UPropHuntGameInstance* GI = Cast<UPropHuntGameInstance>(GetGameInstance()))
        {
            GI->SavedRole = EPlayerRole::Hunter;
        }
		
        PC->Server_RequestRole(EPlayerRole::Hunter);
    }
}

void UPropHuntMenu::OnPropClicked()
{
    if (APropHuntPlayerController* PC = Cast<APropHuntPlayerController>(GetOwningPlayer()))
    {
        if (UPropHuntGameInstance* GI = Cast<UPropHuntGameInstance>(GetGameInstance()))
        {
            GI->SavedRole = EPlayerRole::Prop;
        }
        PC->Server_RequestRole(EPlayerRole::Prop);
    }
}

void UPropHuntMenu::OnStartGameClicked()
{
    if (APropHuntPlayerController* PC = Cast<APropHuntPlayerController>(GetOwningPlayer()))
    {
        PC->Server_StartMatch(); 
    }
}
