// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PropHuntJOchoaCharacter.h"
#include "PropHuntGameInstance.generated.h"

UCLASS()
class PROPHUNTJOCHOA_API UPropHuntGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Prop Hunt")
	EPlayerRole SavedRole = EPlayerRole::Unassigned;
};
