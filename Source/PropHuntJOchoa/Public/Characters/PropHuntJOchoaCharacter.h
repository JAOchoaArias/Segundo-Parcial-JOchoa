// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Core/PropHuntTypes.h"
#include "PropHuntJOchoaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UStaticMeshComponent;
class UStaticMesh;
struct FInputActionValue;
class UHealthComponent;
class UNiagaraSystem;
class USoundBase;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class APropHuntJOchoaCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Prop Hunt", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> DisguiseMeshComponent;
	
protected:
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AttackAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LockPropAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RotatePropAction;

	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> PropLockSound;
	
	UPROPERTY(ReplicatedUsing = OnRep_PropLockTransform, BlueprintReadOnly, Category = "Prop Hunt")
	bool bIsPropLocked = false;

	UPROPERTY(ReplicatedUsing = OnRep_PropLockTransform, BlueprintReadOnly, Category = "Prop Hunt")
	FVector LockedMeshOffset = FVector::ZeroVector;
	
	UPROPERTY(ReplicatedUsing = OnRep_PropLockTransform, BlueprintReadOnly, Category = "Prop Hunt")
	float LockedMeshYaw = 0.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentDisguiseMesh, BlueprintReadOnly, Category = "Prop Hunt")
	TObjectPtr<UStaticMesh> CurrentDisguiseMesh;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Prop Hunt")
	FVector CurrentDisguiseScale = FVector::OneVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;
	
	UFUNCTION()
	void OnRep_Role();
	
	UFUNCTION()
	void OnRep_CurrentDisguiseMesh();
	
	UFUNCTION()
	void HandleMatchPhaseChanged(EMatchState NewState, EPlayerRole Winner);

	void UpdateHidingRestrictions();
	
	UFUNCTION()
	void OnRep_PropLockTransform();

	void TogglePropLock();
	void RotateProp(const FInputActionValue& Value);
	void OnPropAdjustCompleted();
	void ApplyPropVisualTransform();
	
	float BaseDisguiseZ = 0.0f;
	float LastPropSyncTime = 0.0f;
	
public:

	/** Constructor */
	APropHuntJOchoaCharacter();	
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TryInteract();
	void PerformHunterAttack();
	
	UFUNCTION(Server, Reliable)
	void Server_SetDisguise(UStaticMesh* NewMesh, FVector NewScale);
	
	UFUNCTION(Server, Reliable)
	void Server_ExecuteAttack(const FVector& TraceStart, const FVector& TraceEnd);
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayTransformEffects(FVector EffectLocation);
	
	UFUNCTION(Server, Reliable)
	void Server_SetPropLockState(bool bNewLocked, FVector NewOffset, float NewYaw);

	UFUNCTION(Server, Unreliable)
	void Server_UpdatePropTransform(FVector NewOffset, float NewYaw);
	
	UPROPERTY(ReplicatedUsing = OnRep_Role, BlueprintReadOnly, Category = "Prop Hunt")
	EPlayerRole CurrentRole = EPlayerRole::Unassigned;
	
	void SetRole(EPlayerRole NewRole);
	
protected:
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void UpdateCapsuleDimensions(UStaticMesh* Mesh2, FVector& Scale);
	
	UFUNCTION()
	void HandleCharacterDeath(AActor* DeadActor, AController* KillerController);
	
	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|VFX")
	TObjectPtr<UNiagaraSystem> TransformVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|VFX")
	TObjectPtr<UNiagaraSystem> HunterBeamVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> TransformSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Prop Hunt|Audio")
	TObjectPtr<USoundBase> HunterAttackSound;

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayAttackEffects(FVector StartLocation, FVector EndLocation);

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UStaticMeshComponent* GetDisguiseMesh() const { return DisguiseMeshComponent; }
};

