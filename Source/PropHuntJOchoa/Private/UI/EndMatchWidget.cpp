#include "UI/EndMatchWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Framework/PropHuntGameState.h"
#include "Framework/PropHuntPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UEndMatchWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_ReturnLobby)
	{
		Btn_ReturnLobby->OnClicked.AddDynamic(this, &UEndMatchWidget::OnReturnLobbyClicked);
	}
	if (Btn_ReturnMenu)
	{
		Btn_ReturnMenu->OnClicked.AddDynamic(this, &UEndMatchWidget::OnReturnMenuClicked);
	}
	APropHuntGameState* GS = GetWorld() ? GetWorld()->GetGameState<APropHuntGameState>() : nullptr;
	if (!GS)
	{
		return;
	}
	const EPlayerRole Winner = GS->WinningTeam;
	const EPlayerRole Loser = (Winner == EPlayerRole::Hunter) ? EPlayerRole::Prop : EPlayerRole::Hunter;
	const FString WinnerName = (Winner == EPlayerRole::Hunter) ? TEXT("CAZADORES") : TEXT("PROPS");
	const FString LoserName = (Loser == EPlayerRole::Hunter) ? TEXT("CAZADORES") : TEXT("PROPS");
	if (Txt_WinnerTeam)
	{
		Txt_WinnerTeam->SetText(FText::FromString(FString::Printf(TEXT("GANADORES: %s"), *WinnerName)));
	}
	if (Txt_LoserTeam)
	{
		Txt_LoserTeam->SetText(FText::FromString(FString::Printf(TEXT("PERDEDORES: %s"), *LoserName)));
	}
	EPlayerRole LocalRole = EPlayerRole::Unassigned;
	if (APropHuntPlayerState* PS = GetOwningPlayerState<APropHuntPlayerState>())
	{
		LocalRole = PS->PlayerRole;
	}
	else if (APropHuntJOchoaCharacter* MyChar = Cast<APropHuntJOchoaCharacter>(GetOwningPlayerPawn()))
	{
		LocalRole = MyChar->CurrentRole;
	}
	const bool bLocalWon = (LocalRole != EPlayerRole::Unassigned && LocalRole == Winner);
	if (Txt_PersonalResult)
	{
		Txt_PersonalResult->SetText(FText::FromString(bLocalWon ? TEXT("¡VICTORIA!") : TEXT("DERROTA")));
		Txt_PersonalResult->SetColorAndOpacity(bLocalWon ? FLinearColor::Green : FLinearColor::Red);
	}
	if (USoundBase* ResultSound = bLocalWon ? VictorySound : DefeatSound)
	{
		UGameplayStatics::PlaySound2D(this, ResultSound);
	}
	OnMatchResultDetermined(Winner, Loser, bLocalWon);
}

void UEndMatchWidget::OnReturnMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Lvl_MainMenuMap"));
}

void UEndMatchWidget::OnReturnLobbyClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel("/Game/Maps/Lvl_LobbyMap?listen");
		}
	}
}