#include "Gameplay/Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	if (AActor* MyOwner = GetOwner())
	{
		if (MyOwner->HasAuthority())
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}
	
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	LastInstigatorController = InstigatedBy;
	
	OnRep_CurrentHealth();

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnRep_IsDead();
	}
}

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(this, CurrentHealth, MaxHealth);
}

void UHealthComponent::OnRep_IsDead()
{
	if (bIsDead)
	{
		OnDeath.Broadcast(GetOwner(), LastInstigatorController);
	}
}