#include "UI/EndMatchWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Framework/PropHuntGameState.h"
#include "Framework/PropHuntPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UEndMatchWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_ReturnMenu)
	{
		Btn_ReturnMenu->OnClicked.AddDynamic(this, &UEndMatchWidget::OnReturnMenuClicked);
	}

	APropHuntGameState* GS = GetWorld()->GetGameState<APropHuntGameState>();
	APropHuntPlayerState* LocalPS = GetOwningPlayerState<APropHuntPlayerState>();

	if (GS)
	{
		if (Txt_WinnerTeam)
		{
			FString WinnerStr = (GS->WinningTeam == EPlayerRole::Hunter) ? TEXT("Equipo Ganador: CAZADORES") : TEXT("Equipo Ganador: PROPS");
			Txt_WinnerTeam->SetText(FText::FromString(WinnerStr));
		}
		
		if (Txt_ResultTitle && LocalPS)
		{
			const bool bWon = (LocalPS->PlayerRole == GS->WinningTeam);
			Txt_ResultTitle->SetText(FText::FromString(bWon ? TEXT("¡VICTORIA!") : TEXT("DERROTA")));
		}
		
		if (Txt_Scoreboard)
		{
			FString ScoreList;
			for (APlayerState* PS : GS->PlayerArray)
			{
				if (APropHuntPlayerState* PH_PS = Cast<APropHuntPlayerState>(PS))
				{
					FString RoleTag = (PH_PS->PlayerRole == EPlayerRole::Hunter) ? TEXT("Cazador") : TEXT("Prop");
					ScoreList += FString::Printf(TEXT("%s (%s) - Puntos: %.0f | Bajas: %d | Muertes: %d\n"),
						*PH_PS->GetPlayerName(), *RoleTag, PH_PS->GetScore(), PH_PS->Kills, PH_PS->Deaths);
				}
			}
			Txt_Scoreboard->SetText(FText::FromString(ScoreList));
		}
	}
}

void UEndMatchWidget::OnReturnMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenuMap"));
}