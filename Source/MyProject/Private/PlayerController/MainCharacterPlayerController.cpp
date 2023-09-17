// EvoAnd Copyright


#include "PlayerController/MainCharacterPlayerController.h"
#include "EnhancedInputSubSystems.h"
#include "Components/InputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Input/MPInputComponent.h"
#include "Character/MainCharacterMovementComponent.h"
#include "Component/CombatComponent.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "MPGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"


AMainCharacterPlayerController::AMainCharacterPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AMainCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(MainCharacterMappingContext);
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(MainCharacterMappingContext, 0);

		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Default;
		UGameplayStatics::SetViewportMouseCaptureMode(this, EMouseCaptureMode::CaptureDuringRightMouseDown);

		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(true);
		SetInputMode(InputModeData);
	}

	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetPawn()))
	{
		MainCharacter->SetPlayerController(this);
	}
}

void AMainCharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UMPInputComponent* MPInputComponent = CastChecked<UMPInputComponent>(InputComponent);
	MPInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	MPInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	MPInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);
	MPInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	MPInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	MPInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ThisClass::EquipUnEquip);
	MPInputComponent->BindAction(BasicComboAttackAction, ETriggerEvent::Started, this, &ThisClass::BasicComboAttack);
	MPInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &ThisClass::Climb);
	MPInputComponent->BindAction(CancelClimbAction, ETriggerEvent::Started, this, &ThisClass::CancelClimb);
	MPInputComponent->BindAction(ClimbDashing, ETriggerEvent::Started, this, &ThisClass::ClimbDash);

	MPInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AMainCharacterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


}

void AMainCharacterPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	//AutoRun();	
}

void AMainCharacterPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockHit, bCriticalHit);
	}
}

void AMainCharacterPlayerController::Move(const FInputActionValue& Value)
{
	//const FVector2D InputAxisVector = Value.Get<FVector2D>();
	//const FRotator Rotation = GetControlRotation();
	//const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	//const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//if (APawn* ControlledPawn = GetPawn<APawn>())
	//{
	//	ControlledPawn->AddMovementInput(ForwardVector, InputAxisVector.Y);
	//	ControlledPawn->AddMovementInput(RightVector, InputAxisVector.X);
	//}

	if (OwnerMainCharacter == nullptr) return;

	if (OwnerMainCharacter->GetActionState() > EActionState::EAS_Run) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(OwnerMainCharacter->GetActorForwardVector().Rotation());

	FVector ForwardDirection, RightDirection;

	UMainCharacterMovementComponent* MovementComponent = Cast<UMainCharacterMovementComponent>(OwnerMainCharacter->GetMovementComponent());
	if (MovementComponent->IsClimbing())
	{
		ForwardDirection = FVector::CrossProduct(MovementComponent->GetClimbSurfaceNormal(), -(OwnerMainCharacter->GetActorRightVector()));
		RightDirection = FVector::CrossProduct(MovementComponent->GetClimbSurfaceNormal(), OwnerMainCharacter->GetActorUpVector());
	}
	else
	{
		ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	}

	OwnerMainCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
	OwnerMainCharacter->AddMovementInput(RightDirection, MovementVector.X);

	FVector TmpMoveVec(ForwardDirection * MovementVector.Y);
	TmpMoveVec = TmpMoveVec + (RightDirection * MovementVector.X);
	OwnerMainCharacter->SetMoveVector(TmpMoveVec.GetSafeNormal());
}

void AMainCharacterPlayerController::Look(const FInputActionValue& Value)
{
	if (OwnerMainCharacter == nullptr) return;

	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	OwnerMainCharacter->AddControllerYawInput(LookAxisValue.X);
	OwnerMainCharacter->AddControllerPitchInput(LookAxisValue.Y);
}

void AMainCharacterPlayerController::Climb()
{
	if (HasAuthority())
		InputClimb();
	else
		ServerClimb();
}

void AMainCharacterPlayerController::InputClimb()
{
	if (OwnerMainCharacterMovementComponent == nullptr || OwnerMainCharacter->GetCharacterState() != ECharacterState::ECS_UnEquipped) return;

	if (OwnerMainCharacterMovementComponent->TryClimbing())
	{
		OwnerMainCharacter->SetActionState(EActionState::EAS_ChangingClimbing);
	}
}

void AMainCharacterPlayerController::ServerClimb_Implementation()
{
	InputClimb();
}

void AMainCharacterPlayerController::CancelClimb()
{
	if (HasAuthority())
		InputCancelClimb();
	else
		ServerCancelClimb();
}

void AMainCharacterPlayerController::InputCancelClimb()
{
	if (OwnerMainCharacterMovementComponent == nullptr || OwnerMainCharacter->GetCharacterState() != ECharacterState::ECS_UnEquipped) return;

	OwnerMainCharacterMovementComponent->CancelClimbing();
}

void AMainCharacterPlayerController::ServerCancelClimb_Implementation()
{
	InputCancelClimb();
}

void AMainCharacterPlayerController::Jump()
{
	if (OwnerMainCharacter) OwnerMainCharacter->Jump();
}

void AMainCharacterPlayerController::EquipUnEquip()
{
	if (OwnerMainCharacter == nullptr) return;
		
	if (OwnerMainCharacter->GetActionState() <= EActionState::EAS_Run)
	{
		if (OwnerMainCharacter->GetCharacterState() == ECharacterState::ECS_UnEquipped)
		{
			OwnerMainCharacter->SetActionState(EActionState::EAS_EquippingWeapon);
		}
		else if (OwnerMainCharacter->GetCharacterState() == ECharacterState::ECS_KatanaCombat)
		{
			OwnerMainCharacter->SetActionState(EActionState::EAS_UnequippingWeapon);
		}
	}
}

void AMainCharacterPlayerController::BasicComboAttack(const FInputActionValue& Value)
{
	if (HasAuthority())
		InputBasicComboAttack(Value);
	else
		ServerBasicComboAttack(Value);
}

void AMainCharacterPlayerController::InputBasicComboAttack(const FInputActionValue& Value)
{
	if (OwnerMainCharacter == nullptr) return;

	UCombatComponent* CombatComponent = OwnerMainCharacter->GetCombatComponent();
	if (CombatComponent && OwnerMainCharacter->GetCharacterState() == ECharacterState::ECS_KatanaCombat)
	{
		CombatComponent->SetNextComboState();
		CombatComponent->PlayNextCombo();
		CombatComponent->SetbCanComboAttack(false);
		OwnerMainCharacter->bCanJump = false;
		OwnerMainCharacter->SetActionState(EActionState::EAS_Attacking);
	}
}

void AMainCharacterPlayerController::ServerBasicComboAttack_Implementation(const FInputActionValue& Value)
{
	InputBasicComboAttack(Value);
}

void AMainCharacterPlayerController::ClimbDash()
{
	if (OwnerMainCharacterMovementComponent && OwnerMainCharacter && OwnerMainCharacter->GetActionState() == EActionState::EAS_Climb)
	{
		OwnerMainCharacterMovementComponent->TryClimbDashing();
		OwnerMainCharacter->SetActionState(EActionState::EAS_ClimbDash);
	}
}

void AMainCharacterPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != ThisActor)
	{
		if(LastActor) LastActor->UnHightlightActor();
		if(ThisActor) ThisActor->HightlightActor();		
	}
}

void AMainCharacterPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FMPGameplayTags::Get().InputTag_RMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AMainCharacterPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FMPGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC())	GetASC()->AbilityInputTagReleased(InputTag);

		return;
	}

	if (GetASC())	GetASC()->AbilityInputTagReleased(InputTag);

	/*if (!bTargeting && !bShiftKeyDown)
	{
		APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				if(NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}
		}

		FollowTime = 0.f;
		bTargeting = false;
	}*/
}

void AMainCharacterPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FMPGameplayTags::Get().InputTag_RMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);		
	}
	/*else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}*/
}

void AMainCharacterPlayerController::SetOwnerMainCharacter(AMainCharacter* InOwner)
{
	OwnerMainCharacter = InOwner;
	OwnerMainCharacterMovementComponent = Cast<UMainCharacterMovementComponent>(OwnerMainCharacter->GetMovementComponent());
}

UMPAbilitySystemComponent* AMainCharacterPlayerController::GetASC()
{
	if (MPAbilitySystemComponent == nullptr)
	{
		MPAbilitySystemComponent = Cast<UMPAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return MPAbilitySystemComponent;
}

void AMainCharacterPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptRadius)
		{
			bAutoRunning = false;
		}
	}
}
