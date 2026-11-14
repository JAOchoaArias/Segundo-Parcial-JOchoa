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
	const FText ResultText = FText::FromString(bLocalWon ? TEXT("¡VICTORIA!") : TEXT("DERROTA"));
	const FLinearColor ResultColor = bLocalWon ? FLinearColor::Green : FLinearColor::Red;
	if (Txt_ResultTitle)
	{
		Txt_ResultTitle->SetText(ResultText);
		Txt_ResultTitle->SetColorAndOpacity(ResultColor);
	}
	if (Txt_PersonalResult)
	{
		Txt_PersonalResult->SetText(ResultText);
		Txt_PersonalResult->SetColorAndOpacity(ResultColor);
	}
	if (Txt_Scoreboard)
	{
		FString ScoreboardText = TEXT("TABLA DE JUGADORES:\n\n");

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (APropHuntPlayerState* PH_PS = Cast<APropHuntPlayerState>(PS))
			{
				const FString RoleStr = (PH_PS->PlayerRole == EPlayerRole::Hunter) ? TEXT("Cazador")
				                      : (PH_PS->PlayerRole == EPlayerRole::Prop)   ? TEXT("Prop")
				                                                                   : TEXT("Sin rol");
				const FString StatusStr = PH_PS->bIsAlive ? TEXT("Vivo") : TEXT("Eliminado");
				const int32 Points = FMath::RoundToInt(PH_PS->GetScore());

				ScoreboardText += FString::Printf(
					TEXT("%s  |  %s  |  %s  |  Puntos: %d\n"),
					*PH_PS->GetPlayerName(),
					*RoleStr,
					*StatusStr,
					Points
				);
			}
		}
		Txt_Scoreboard->SetText(FText::FromString(ScoreboardText));
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