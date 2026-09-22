// Copyright Epic Games, Inc. All Rights Reserved.

#include "PropHuntJOchoaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Public/Interfaces/BPI_Interactable.h"
#include "PropHuntJOchoa.h"
#include "Net/UnrealNetwork.h"


APropHuntJOchoaCharacter::APropHuntJOchoaCharacter()
{
	bReplicates = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	DisguiseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisguiseMeshComponent"));
	DisguiseMeshComponent->SetupAttachment(GetCapsuleComponent());
	DisguiseMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisguiseMeshComponent->SetVisibility(false);
}

void APropHuntJOchoaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APropHuntJOchoaCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APropHuntJOchoaCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APropHuntJOchoaCharacter::Look);
	}
	else
	{
		UE_LOG(LogPropHuntJOchoa, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APropHuntJOchoaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APropHuntJOchoaCharacter, CurrentRole);
	DOREPLIFETIME(APropHuntJOchoaCharacter, CurrentDisguiseMesh);
}

void APropHuntJOchoaCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void APropHuntJOchoaCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void APropHuntJOchoaCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void APropHuntJOchoaCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APropHuntJOchoaCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void APropHuntJOchoaCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void APropHuntJOchoaCharacter::TryInteract()
{
	if (CurrentRole != EPlayerRole::Prop)
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	GetActorEyesViewPoint(CamLoc, CamRot);

	const FVector TraceEnd = CamLoc + (CamRot.Vector() * 450.0f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, CamLoc, TraceEnd, ECC_Visibility, Params))
	{
		if (HitResult.GetActor() && HitResult.GetActor()->Implements<UBPI_Interactable>())
		{
			IBPI_Interactable* Interactable = Cast<IBPI_Interactable>(HitResult.GetActor());
			if (Interactable)
			{
				FVector Loc, Scale;
				FRotator Rot;
				Interactable->GetPropMeshTransform(Loc, Rot, Scale);
				Server_SetDisguise(Interactable->GetPropMesh(), Scale);
			}
		}
	}
}

void APropHuntJOchoaCharacter::PerformHunterAttack()
{
	if (CurrentRole != EPlayerRole::Hunter)
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	GetActorEyesViewPoint(CamLoc, CamRot);

	const FVector TraceEnd = CamLoc + (CamRot.Vector() * 500.0f);
	Server_ExecuteAttack(CamLoc, TraceEnd);
}

void APropHuntJOchoaCharacter::Server_SetDisguise_Implementation(UStaticMesh* NewMesh, FVector NewScale)
{
	if (!NewMesh)
	{
		return;
	}

	CurrentDisguiseMesh = NewMesh;
	DisguiseMeshComponent->SetWorldScale3D(NewScale);
	UpdateCapsuleDimensions(NewMesh, NewScale);
	
	OnRep_CurrentDisguiseMesh();
}

void APropHuntJOchoaCharacter::Server_ExecuteAttack_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

	if (bHit && HitResult.GetActor())
	{
		APropHuntJOchoaCharacter* TargetCharacter = Cast<APropHuntJOchoaCharacter>(HitResult.GetActor());
		if (TargetCharacter && TargetCharacter->CurrentRole == EPlayerRole::Prop)
		{
			TargetCharacter->TakeDamage(25.0f, FDamageEvent(), GetController(), this);
			return;
		}
	}
	
	TakeDamage(5.0f, FDamageEvent(), GetController(), this);
}

void APropHuntJOchoaCharacter::OnRep_Role()
{
	if (CurrentRole == EPlayerRole::Hunter)
	{
		DisguiseMeshComponent->SetVisibility(false);
		GetMesh()->SetVisibility(true);
	}
	else if (CurrentRole == EPlayerRole::Prop && CurrentDisguiseMesh)
	{
		GetMesh()->SetVisibility(false);
		DisguiseMeshComponent->SetVisibility(true);
	}
}

void APropHuntJOchoaCharacter::OnRep_CurrentDisguiseMesh()
{
	if (CurrentDisguiseMesh)
	{
		GetMesh()->SetVisibility(false);
		DisguiseMeshComponent->SetStaticMesh(CurrentDisguiseMesh);
		DisguiseMeshComponent->SetVisibility(true);
	}
	else
	{
		GetMesh()->SetVisibility(true);
		DisguiseMeshComponent->SetVisibility(false);
	}
}

void APropHuntJOchoaCharacter::UpdateCapsuleDimensions(UStaticMesh* Mesh, FVector& Scale)
{
	if (!Mesh)
	{
		return;
	}

	const FBoxSphereBounds Bounds = Mesh->GetBounds();
	const float Radius = FMath::Max(Bounds.BoxExtent.X * Scale.X, Bounds.BoxExtent.Y * Scale.Y);
	const float HalfHeight = FMath::Max(Radius, Bounds.BoxExtent.Z * Scale.Z);

	GetCapsuleComponent()->SetCapsuleSize(
		FMath::Clamp(Radius, 20.0f, 150.0f),
		FMath::Clamp(HalfHeight, 30.0f, 250.0f)
	);
}
