// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Actors/PropBase.h"


// Sets default values
APropBase::APropBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	PropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMeshComponent"));
	RootComponent = PropMeshComponent;
	PropMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void APropBase::GetPropMeshTransform(FVector& OutLocation, FRotator& OutRotation, FVector& OutScale) const
{
	OutLocation = PropMeshComponent->GetRelativeLocation();
	OutRotation = PropMeshComponent->GetRelativeRotation();
	OutScale = PropMeshComponent->GetComponentScale();
}

// Called when the game starts or when spawned
void APropBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APropBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

