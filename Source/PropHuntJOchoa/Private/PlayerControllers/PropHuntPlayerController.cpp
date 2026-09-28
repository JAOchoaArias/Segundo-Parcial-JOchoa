// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PropHuntPlayerController.h"
#include "PropHuntJOchoaCharacter.h"
#include "GameFramework/GameModeBase.h"

void APropHuntPlayerController::Server_RequestRole_Implementation(EPlayerRole RequestedRole)
{
	if (APropHuntJOchoaCharacter* MyCharacter = Cast<APropHuntJOchoaCharacter>(GetPawn()))
	{
		MyCharacter->SetRole(RequestedRole); 
	}
}

void APropHuntPlayerController::Server_StartMatch_Implementation()
{
	if (HasAuthority())
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Lvl_ThirdPerson?listen");
	}
}
