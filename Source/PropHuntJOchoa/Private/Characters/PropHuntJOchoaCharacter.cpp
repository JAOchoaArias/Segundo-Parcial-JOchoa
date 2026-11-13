// Copyright Epic Games, Inc. All Rights Reserved.
#include "Characters/PropHuntJOchoaCharacter.h"
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
#include "Public/Gameplay/Interfaces/BPI_Interactable.h"
#include "PropHuntJOchoa.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Public/Core/PropHuntGameInstance.h"
#include "Public/Framework/PropHuntPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Framework/PropHuntPlayerState.h"
#include "Framework/PropHuntGameMode.h"
#include "Public/Framework/PropHuntGameState.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"


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
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
		
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APropHuntJOchoaCharacter::TryInteract);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APropHuntJOchoaCharacter::PerformHunterAttack);
		
		//Lock camera from prop
		if (LockPropAction)
		{
			EnhancedInputComponent->BindAction(LockPropAction, ETriggerEvent::Started, this, &APropHuntJOchoaCharacter::TogglePropLock);
		}
		
		//rotate prop in lock mode
		if (RotatePropAction)
		{
			EnhancedInputComponent->BindAction(RotatePropAction, ETriggerEvent::Triggered, this, &APropHuntJOchoaCharacter::RotateProp);
			EnhancedInputComponent->BindAction(RotatePropAction, ETriggerEvent::Completed, this, &APropHuntJOchoaCharacter::OnPropAdjustCompleted);
		}
		
		//Move prop in lock mode
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APropHuntJOchoaCharacter::OnPropAdjustCompleted);
		}
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
	DOREPLIFETIME(APropHuntJOchoaCharacter, CurrentDisguiseScale);
	DOREPLIFETIME(APropHuntJOchoaCharacter, bIsPropLocked);
	DOREPLIFETIME(APropHuntJOchoaCharacter, LockedMeshOffset);
	DOREPLIFETIME(APropHuntJOchoaCharacter, LockedMeshYaw);
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

void APropHuntJOchoaCharacter::UpdateCapsuleDimensions(UStaticMesh* Mesh2, FVector& Scale)
{
	if (!Mesh2 || !GetCapsuleComponent() || !DisguiseMeshComponent)
	{
		return;
	}

	const float OldHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	
	const FBox LocalBox = Mesh2->GetBoundingBox();
	const FVector ScaledSize = LocalBox.GetSize() * Scale;
	
	const float NewRadius = FMath::Clamp(FMath::Max(ScaledSize.X, ScaledSize.Y) * 0.5f, 15.0f, 100.0f);
	const float NewHalfHeight = FMath::Max(ScaledSize.Z * 0.5f, NewRadius);

	GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHalfHeight);
	
	const float MeshBottomOffset = LocalBox.Min.Z * Scale.Z;
	const float TargetRelativeZ = -NewHalfHeight - MeshBottomOffset;
	BaseDisguiseZ = TargetRelativeZ;
	ApplyPropVisualTransform();
	if (HasAuthority())
	{
		const float HeightDiff = OldHalfHeight - NewHalfHeight;
		if (HeightDiff > 0.0f)
		{
			AddActorWorldOffset(FVector(0.0f, 0.0f, -HeightDiff), true);
		}
	}
}

void APropHuntJOchoaCharacter::DoMove(float Right, float Forward)
{
	if (CurrentRole == EPlayerRole::Prop && bIsPropLocked)
	{
		if (Controller && DisguiseMeshComponent)
		{
			const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
			const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			const FVector WorldDelta = (ForwardDir * Forward + RightDir * Right) * 80.0f * GetWorld()->GetDeltaSeconds();
			const FVector LocalDelta = GetActorTransform().InverseTransformVectorNoScale(WorldDelta);
			
			LockedMeshOffset.X = FMath::Clamp(LockedMeshOffset.X + LocalDelta.X, -45.0f, 45.0f);
			LockedMeshOffset.Y = FMath::Clamp(LockedMeshOffset.Y + LocalDelta.Y, -45.0f, 45.0f);

			ApplyPropVisualTransform();
			
			const float Now = GetWorld()->GetTimeSeconds();
			if (Now - LastPropSyncTime >= 0.1f)
			{
				LastPropSyncTime = Now;
				Server_UpdatePropTransform(LockedMeshOffset, LockedMeshYaw);
			}
		}
		return;
	}
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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
	if (CurrentRole == EPlayerRole::Prop && bIsPropLocked)
	{
		return;
	}
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
	
    DrawDebugLine(GetWorld(), CamLoc, TraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, CamLoc, TraceEnd, ECC_Visibility, Params))
    {
       if (HitResult.bBlockingHit) 
       {
          DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(10.f), FColor::Green, false, 2.0f);
    
          if (HitResult.GetActor())
          {
             FString NombreObjeto = HitResult.GetActor()->GetName();
          }
       }
       
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
	if (CurrentRole != EPlayerRole::Hunter || !FollowCamera)
	{
		return;
	}

	APropHuntGameState* GS = GetWorld() ? GetWorld()->GetGameState<APropHuntGameState>() : nullptr;
	if (!GS || GS->CurrentMatchState != EMatchState::InProgress)
	{
		return;
	}

	const float AttackRange = 1500.0f;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	const FVector CamStart = FollowCamera->GetComponentLocation();
	const FVector CamEnd = CamStart + (FollowCamera->GetForwardVector() * AttackRange);

	FHitResult CamHit;
	const bool bCamHit = GetWorld()->LineTraceSingleByChannel(CamHit, CamStart, CamEnd, ECC_Visibility, QueryParams);
	const FVector AimTarget = bCamHit ? CamHit.ImpactPoint : CamEnd;
	
	const FVector CharStart = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	const FVector ShotDirection = (AimTarget - CharStart).GetSafeNormal();
	const FVector CharEnd = CharStart + (ShotDirection * AttackRange);

	Server_ExecuteAttack(CharStart, CharEnd);
}

void APropHuntJOchoaCharacter::Server_SetDisguise_Implementation(UStaticMesh* NewMesh, FVector NewScale)
{
	if (!NewMesh)
	{
		return;
	}

	CurrentDisguiseMesh = NewMesh;
	CurrentDisguiseScale = NewScale;
	CurrentDisguiseMesh = NewMesh;
	CurrentDisguiseScale = NewScale;
	LockedMeshOffset = FVector::ZeroVector;
	
	OnRep_CurrentDisguiseMesh();
	
	Multicast_PlayTransformEffects(GetActorLocation());
}

void APropHuntJOchoaCharacter::Server_ExecuteAttack_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	APropHuntGameState* GS = GetWorld() ? GetWorld()->GetGameState<APropHuntGameState>() : nullptr;
	if (!GS || GS->CurrentMatchState != EMatchState::InProgress)
	{
		return;
	}
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult HitResult;
	const bool bHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	const FVector ActualEnd = bHitSomething ? HitResult.ImpactPoint : TraceEnd;
	Multicast_PlayAttackEffects(TraceStart, ActualEnd);
	bool bHitProp = false;
	if (bHitSomething)
	{
		if (APropHuntJOchoaCharacter* HitChar = Cast<APropHuntJOchoaCharacter>(HitResult.GetActor()))
		{
			if (HitChar->CurrentRole == EPlayerRole::Prop)
			{
				bHitProp = true;
				UGameplayStatics::ApplyDamage(HitChar, 25.0f, GetController(), this, nullptr);
			}
		}
	}
	if (!bHitProp)
	{
		UGameplayStatics::ApplyDamage(this, 5.0f, GetController(), this, nullptr);
	}
	if (APropHuntPlayerController* PC = Cast<APropHuntPlayerController>(GetController()))
	{
		PC->Client_NotifyAttackResult(bHitProp);
	}
}

void APropHuntJOchoaCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &APropHuntJOchoaCharacter::HandleCharacterDeath);
	}
	
	if (APropHuntGameState* GS = GetWorld()->GetGameState<APropHuntGameState>())
	{
		GS->OnMatchStateChanged.AddDynamic(this, &APropHuntJOchoaCharacter::HandleMatchPhaseChanged);
	}

	UpdateHidingRestrictions();
}

void APropHuntJOchoaCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (IsLocallyControlled())
	{
		if (UPropHuntGameInstance* GI = Cast<UPropHuntGameInstance>(GetGameInstance()))
		{
			if (GI->SavedRole != EPlayerRole::Unassigned)
			{
				if (APropHuntPlayerController* PC = Cast<APropHuntPlayerController>(GetController()))
				{
					PC->Server_RequestRole(GI->SavedRole);
				}
			}
		}
	}
}

void APropHuntJOchoaCharacter::OnRep_Role()
{
	const bool bIsHunter = (CurrentRole == EPlayerRole::Hunter);
	const bool bHasDisguise = (CurrentDisguiseMesh != nullptr);

	if (GetMesh())
	{
		GetMesh()->SetVisibility(bIsHunter || !bHasDisguise);
	}

	if (DisguiseMeshComponent)
	{
		DisguiseMeshComponent->SetVisibility(!bIsHunter);
	}
	
	if (CameraBoom)
	{
		CameraBoom->SocketOffset = bIsHunter ? FVector(0.0f, 55.0f, 20.0f) : FVector::ZeroVector;
	}

	UpdateHidingRestrictions();
}

void APropHuntJOchoaCharacter::OnRep_CurrentDisguiseMesh()
{
	if (CurrentDisguiseMesh && DisguiseMeshComponent)
	{
		GetMesh()->SetVisibility(false);

		DisguiseMeshComponent->SetStaticMesh(CurrentDisguiseMesh);
		DisguiseMeshComponent->SetWorldScale3D(CurrentDisguiseScale);
		DisguiseMeshComponent->SetVisibility(true);
		
		UpdateCapsuleDimensions(CurrentDisguiseMesh, CurrentDisguiseScale);
	}
}

void APropHuntJOchoaCharacter::SetRole(EPlayerRole NewRole)
{
	if (HasAuthority()) 
	{
		CurrentRole = NewRole;
		
		if (APropHuntPlayerState* PH_PS = GetPlayerState<APropHuntPlayerState>())
		{
			PH_PS->SetPlayerRole(NewRole);
		}
       
		OnRep_Role(); 

		if (APropHuntGameState* GS = Cast<APropHuntGameState>(GetWorld()->GetGameState()))
		{
			GS->RecalculateTeams();
		}
	}
}

void APropHuntJOchoaCharacter::HandleCharacterDeath(AActor* DeadActor, AController* KillerController)
{
	GetMesh()->SetVisibility(false);
	DisguiseMeshComponent->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	
	if (HasAuthority())
	{
		if (APropHuntGameMode* GM = GetWorld()->GetAuthGameMode<APropHuntGameMode>())
		{
			GM->OnPlayerEliminated(GetController(), KillerController);
		}
	}
}

void APropHuntJOchoaCharacter::Multicast_PlayTransformEffects_Implementation(FVector EffectLocation)
{
	if (TransformVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TransformVFX, EffectLocation);
	}
	if (TransformSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TransformSound, EffectLocation);
	}
}

void APropHuntJOchoaCharacter::HandleMatchPhaseChanged(EMatchState NewState, EPlayerRole Winner)
{
	UpdateHidingRestrictions();
}

void APropHuntJOchoaCharacter::UpdateHidingRestrictions()
{
	if (HealthComponent && HealthComponent->IsDead())
	{
		return;
	}

	APropHuntGameState* GS = GetWorld() ? GetWorld()->GetGameState<APropHuntGameState>() : nullptr;
	const bool bIsHidingPhase = (GS && GS->CurrentMatchState == EMatchState::Hiding);
	
	const bool bShouldFreeze = bIsHidingPhase && (CurrentRole != EPlayerRole::Prop);

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (bShouldFreeze)
	{
		GetCharacterMovement()->DisableMovement();

		if (IsLocallyControlled() && PC)
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);
			
			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->StartCameraFade(1.0f, 1.0f, 0.01f, FLinearColor::Black, false, true);
			}
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		if (IsLocallyControlled() && PC)
		{
			PC->ResetIgnoreMoveInput();
			PC->ResetIgnoreLookInput();
			
			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 0.5f, FLinearColor::Black, false, false);
			}
		}
	}
}

void APropHuntJOchoaCharacter::Multicast_PlayAttackEffects_Implementation(FVector StartLocation, FVector EndLocation)
{
	if (HunterAttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HunterAttackSound, StartLocation);
	}

	if (HunterBeamVFX)
	{
		if (UNiagaraComponent* BeamComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HunterBeamVFX, StartLocation))
		{
			BeamComp->SetVectorParameter(FName("BeamStart"), StartLocation);
			BeamComp->SetVectorParameter(FName("BeamEnd"), EndLocation);
		}
	}
}

void APropHuntJOchoaCharacter::TogglePropLock()
{
	if (CurrentRole != EPlayerRole::Prop)
	{
		return;
	}
	bIsPropLocked = !bIsPropLocked;
	if (bIsPropLocked)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		LockedMeshOffset = FVector::ZeroVector;
		LockedMeshYaw = 0.0f;
	}
	ApplyPropVisualTransform();

	if (PropLockSound)
	{
		UGameplayStatics::PlaySound2D(this, PropLockSound);
	}
	Server_SetPropLockState(bIsPropLocked, LockedMeshOffset, LockedMeshYaw);
}

void APropHuntJOchoaCharacter::RotateProp(const FInputActionValue& Value)
{
	if (CurrentRole != EPlayerRole::Prop || !bIsPropLocked)
	{
		return;
	}
	const float AxisValue = Value.Get<float>();
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;
	}
	LockedMeshYaw = FMath::Fmod(LockedMeshYaw + (AxisValue * 120.0f * GetWorld()->GetDeltaSeconds()), 360.0f);
	ApplyPropVisualTransform();

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastPropSyncTime >= 0.1f)
	{
		LastPropSyncTime = Now;
		Server_UpdatePropTransform(LockedMeshOffset, LockedMeshYaw);
	}
}

void APropHuntJOchoaCharacter::OnPropAdjustCompleted()
{
	if (CurrentRole == EPlayerRole::Prop && bIsPropLocked)
	{
		Server_SetPropLockState(bIsPropLocked, LockedMeshOffset, LockedMeshYaw);
	}
}

void APropHuntJOchoaCharacter::ApplyPropVisualTransform()
{
	if (DisguiseMeshComponent)
	{
		DisguiseMeshComponent->SetRelativeLocation(FVector(LockedMeshOffset.X, LockedMeshOffset.Y, BaseDisguiseZ));
		DisguiseMeshComponent->SetRelativeRotation(FRotator(0.0f, LockedMeshYaw, 0.0f));
	}
}

void APropHuntJOchoaCharacter::OnRep_PropLockTransform()
{
	ApplyPropVisualTransform();
}

void APropHuntJOchoaCharacter::Server_SetPropLockState_Implementation(bool bNewLocked, FVector NewOffset, float NewYaw)
{
	bIsPropLocked = bNewLocked;
	LockedMeshOffset.X = FMath::Clamp(NewOffset.X, -45.0f, 45.0f);
	LockedMeshOffset.Y = FMath::Clamp(NewOffset.Y, -45.0f, 45.0f);
	LockedMeshYaw = NewYaw;

	if (bIsPropLocked && GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
	ApplyPropVisualTransform();
}

void APropHuntJOchoaCharacter::Server_UpdatePropTransform_Implementation(FVector NewOffset, float NewYaw)
{
	if (!bIsPropLocked)
	{
		return;
	}
	LockedMeshOffset.X = FMath::Clamp(NewOffset.X, -45.0f, 45.0f);
	LockedMeshOffset.Y = FMath::Clamp(NewOffset.Y, -45.0f, 45.0f);
	LockedMeshYaw = NewYaw;

	ApplyPropVisualTransform();
}