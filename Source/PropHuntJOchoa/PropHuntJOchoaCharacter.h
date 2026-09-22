// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PropHuntJOchoaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UStaticMeshComponent;
class UStaticMesh;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Unassigned UMETA(DisplayName = "Unassigned"),
	Hunter UMETA(DisplayName = "Hunter"),
	Prop UMETA(DisplayName = "Prop")
};

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

	
protected:
	
	UPROPERTY(ReplicatedUsing = OnRep_Role, BlueprintReadOnly, Category = "Prop Hunt")
	EPlayerRole CurrentRole = EPlayerRole::Unassigned;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentDisguiseMesh, BlueprintReadOnly, Category = "Prop Hunt")
	TObjectPtr<UStaticMesh> CurrentDisguiseMesh;
	
	UFUNCTION()
	void OnRep_Role();
	
	UFUNCTION()
	void OnRep_CurrentDisguiseMesh();
	
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
	
protected:
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void UpdateCapsuleDimensions(UStaticMesh* Mesh, FVector& Scale);

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

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class UStaticMeshComponent* GetDisguiseMesh() const { return DisguiseMeshComponent; }
};

