// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/Interfaces/BPI_Interactable.h"
#include "PropBase.generated.h"

UCLASS()
class PROPHUNTJOCHOA_API APropBase : public AActor, public IBPI_Interactable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APropBase();
	
	virtual UStaticMesh* GetPropMesh() const override { return PropMeshComponent->GetStaticMesh(); }
	virtual void GetPropMeshTransform(FVector& OutLocation, FRotator& OutRotation, FVector& OutScale) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop")
	TObjectPtr<UStaticMeshComponent> PropMeshComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
