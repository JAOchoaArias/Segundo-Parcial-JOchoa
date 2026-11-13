#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeathSignature, AActor*, DeadActor, AController*, KillerController);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROPHUNTJOCHOA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnDeathSignature OnDeath;
	
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_IsDead();
	
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:
	UPROPERTY()
	TObjectPtr<AController> LastInstigatorController;
};