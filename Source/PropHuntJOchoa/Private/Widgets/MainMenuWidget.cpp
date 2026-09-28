#include "Widgets/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (Btn_Play) Btn_Play->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayClicked);
    if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
    
    if (Btn_GoToMultiplayer) Btn_GoToMultiplayer->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGoToMultiplayerClicked);
    if (Btn_GoToLAN) Btn_GoToLAN->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGoToLANClicked);
    if (Btn_BackToStart) Btn_BackToStart->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackToStartClicked);
    
    if (Btn_HostLAN) Btn_HostLAN->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostLANClicked);
    if (Btn_JoinLAN) Btn_JoinLAN->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinLANClicked);
    if (Btn_BackToModes) Btn_BackToModes->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackToModesClicked);
    
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);
    }
}

void UMainMenuWidget::OnPlayClicked()
{
    if (MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(1); 
}

void UMainMenuWidget::OnGoToLANClicked()
{
    if (MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(2); 
}

void UMainMenuWidget::OnBackToStartClicked()
{
    if (MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(0); 
}

void UMainMenuWidget::OnBackToModesClicked()
{
    if (MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(1); 
}


void UMainMenuWidget::OnGoToMultiplayerClicked()
{
    if (PluginMenuClass && GetWorld())
    {
        UUserWidget* PluginMenu = CreateWidget<UUserWidget>(GetWorld(), PluginMenuClass);
        if (PluginMenu)
        {
            PluginMenu->AddToViewport();
            RemoveFromParent(); 
        }
    }
}

void UMainMenuWidget::OnHostLANClicked()
{
    GetWorld()->ServerTravel("/Game/Levels/LobbyMap?listen"); 
}

void UMainMenuWidget::OnJoinLANClicked()
{
    if (Txt_IPInput && !Txt_IPInput->GetText().IsEmpty())
    {
        FString IPAddress = Txt_IPInput->GetText().ToString();
        if (APlayerController* PC = GetOwningPlayer())
        {
            PC->ClientTravel(IPAddress, ETravelType::TRAVEL_Absolute);
        }
    }
}

void UMainMenuWidget::OnQuitClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
    }
}