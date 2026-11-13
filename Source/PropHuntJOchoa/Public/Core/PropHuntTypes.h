#pragma once

#include "CoreMinimal.h"
#include "PropHuntTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Unassigned UMETA(DisplayName = "Unassigned"),
	Hunter UMETA(DisplayName = "Hunter"),
	Prop UMETA(DisplayName = "Prop")
};

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	Lobby UMETA(DisplayName = "Lobby"),
	Hiding UMETA(DisplayName = "Escondite"),
	InProgress UMETA(DisplayName = "Jugando"),
	MatchEnded UMETA(DisplayName = "Fin")
};