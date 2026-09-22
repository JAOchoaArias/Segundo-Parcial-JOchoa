// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBPI_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROPHUNTJOCHOA_API IBPI_Interactable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UStaticMesh* GetPropMesh() const = 0;
	virtual void GetPropMeshTransform(FVector& OutRelativeLocation, FRotator& OutRelativeRotation, FVector& OutScale) const = 0;
};
