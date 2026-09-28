// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PropHuntJOchoaCharacter.h"
#include "PropHuntPlayerController.generated.h"


UCLASS()
class PROPHUNTJOCHOA_API APropHuntPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(Server, Reliable)
	void Server_RequestRole(EPlayerRole RequestedRole);
	
	UFUNCTION(Server, Reliable)
	void Server_StartMatch();
};
