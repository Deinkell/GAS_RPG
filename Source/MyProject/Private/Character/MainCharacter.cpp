// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Item/Weapon.h"
#include "Component/CombatComponent.h"
#include "Game/MPPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/MainCharacterMovementComponent.h"
#include "PlayerController/MainCharacterPlayerController.h"
#include "UI/HUD/MPHUD.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "Component/InventoryComponent.h"

void LogActionState(EActionState State)
{
	switch (State)
	{
	case EActionState::EAS_Unoccupied:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Unoccupied"));
		break;
	case EActionState::EAS_Walk:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Walk"));
		break;
	case EActionState::EAS_Run:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Run"));
		break;
	case EActionState::EAS_Jump:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Jump"));
		break;
	case EActionState::EAS_HitReaction:
		UE_LOG(LogTemp, Warning, TEXT("EAS_HitReaction"));
		break;
	case EActionState::EAS_Attacking:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Attacking"));
		break;
	case EActionState::EAS_EquippingWeapon:
		UE_LOG(LogTemp, Warning, TEXT("EAS_EquippingWeapon"));
		break;
	case EActionState::EAS_UnequippingWeapon:
		UE_LOG(LogTemp, Warning, TEXT("EAS_UnequippingWeapon"));
		break;
	case EActionState::EAS_Dodge:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Dodge"));
		break;
	case EActionState::EAS_Death:
		UE_LOG(LogTemp, Warning, TEXT("EAS_Death"));
		break;
	}
};

AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UMainCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	MovementComponent = Cast<UMainCharacterMovementComponent>(GetCharacterMovement());
	Initialize();

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	Inventory = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	Inventory->SetInventoryOwner(this);
}

void AMainCharacter::Initialize()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Gloves = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gloves"));
	Gloves->SetupAttachment(GetMesh());
	Gloves->SetLeaderPoseComponent(GetMesh());

	BodyUpper = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyUpper"));
	BodyUpper->SetupAttachment(GetMesh());
	BodyUpper->SetLeaderPoseComponent(GetMesh());

	ShoulderPad = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShoulderPad"));
	ShoulderPad->SetupAttachment(GetMesh());
	ShoulderPad->SetLeaderPoseComponent(GetMesh());

	BodyLower = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyLower"));
	BodyLower->SetupAttachment(GetMesh());
	BodyLower->SetLeaderPoseComponent(GetMesh());

	Foot = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Foot"));
	Foot->SetupAttachment(GetMesh());
	Foot->SetLeaderPoseComponent(GetMesh());

	HeadAccessory = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadAccessory"));
	HeadAccessory->SetupAttachment(GetMesh(), FName(TEXT("Head_Accessories")));

	BackAccessory = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackAccessory"));
	BackAccessory->SetupAttachment(GetMesh(), FName(TEXT("back_accessories")));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 700.f;
	//CameraBoom->bInheritPitch = false;
	//CameraBoom->bInheritRoll = false;
	//CameraBoom->bInheritYaw = false;
	CameraBoom->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetOwnerCharacter(this);
	CombatComponent->SetIsReplicated(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetMovementComponent()->SetIsReplicated(true);
	GetCharacterMovement()->bOrientRotationToMovement = false;  //젤다 Climbing에 설정된 기본 설정
	//GetCharacterMovement()->bOrientRotationToMovement = true; // GAS topdown 설정
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	CharacterClass = ECharacterClass::Elementalist;
}

void AMainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AMainCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
}

void AMainCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = Camera->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation =  (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CombatComponent->SetOwnerCharacter(this);
	MovementComponent->SetOwnerMainCharacter(this);
	MovementComponent->OnCancelClimbDelegate.BindUObject(this, &ThisClass::CancelClimb);
	MovementComponent->OnLedgeUpClimbDelegate.BindUObject(this, &ThisClass::LedgeUpClimbing);
	MovementComponent->OnClimbDashDelegate.BindUObject(this, &ThisClass::ClimbDashEnd);

	if (!bUseDefaultWeapon && WeaponClass && Weapon == nullptr)
	{
		if (HasAuthority())
		{
			Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
			Weapon->SetMPOwner(this);
			UnEquipKatana();
		}	
	}	
}

void AMainCharacter::InitAbilityActorInfo()
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);
	MPPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MPPlayerState, this);
	Cast<UMPAbilitySystemComponent>(MPPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = MPPlayerState->GetAbilitySystemComponent();
	AttributeSet = MPPlayerState->GetAttributeSet();

	if (AMainCharacterPlayerController* MainCharacterPlayerController = Cast<AMainCharacterPlayerController>(GetController()))
	{
		MainCharacterPlayerController->SetOwnerMainCharacter(this);
		if (AMPHUD* MPHUD = Cast<AMPHUD>(MainCharacterPlayerController->GetHUD()))
		{
			MPHUD->InitOverlay(MainCharacterPlayerController, MPPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitializeDefaultAttributes();
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
		PollInit(DeltaTime);
}

bool AMainCharacter::CanCastFirebolt()
{
	return CharacterState == ECharacterState::ECS_KatanaCombat && ActionState == EActionState::EAS_Unoccupied;
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, CharacterState);
	DOREPLIFETIME(AMainCharacter, ActionState);
	DOREPLIFETIME(AMainCharacter, MoveVector); 
	DOREPLIFETIME(AMainCharacter, Movement_Yaw);
	DOREPLIFETIME(AMainCharacter, MCSpeed);
	DOREPLIFETIME(AMainCharacter, bIsInAir);
	DOREPLIFETIME(AMainCharacter, AO_Yaw);
}

void AMainCharacter::SetAllowPhysicsRotationDuringRootMotion(bool bAllow)
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = bAllow;
}

void AMainCharacter::AddToXP_Implementation(int32 InXP)
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	MPPlayerState->AddtoXP(InXP);
}

void AMainCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	MPPlayerState->AddtoLevel(InPlayerLevel);

	if (UMPAbilitySystemComponent* MPASC = Cast<UMPAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		MPASC->UpdateAbilityStatuses(MPPlayerState->GetPlayerLevel());
	}
}

void AMainCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	MPPlayerState->AddtoAttributePoints(InAttributePoints);
}

void AMainCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	MPPlayerState->AddtoSpellPoints(InSpellPoints);
}

void AMainCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AMainCharacter::GetXP_Implementation() const
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->GetXP();
}

int32 AMainCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AMainCharacter::GetAttributePointReward_Implementation(int32 Level) const
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AMainCharacter::GetSpellPointReward_Implementation(int32 Level) const
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

int32 AMainCharacter::GetAttributePoints_Implementation() const
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->GetAttributePoints();
}

int32 AMainCharacter::GetSpellPoints_Implementation() const
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->GetSpellPoints();
}

int32 AMainCharacter::GetPlayerLevel_Implementation()
{
	AMPPlayerState* MPPlayerState = GetPlayerState<AMPPlayerState>();
	check(MPPlayerState);

	return MPPlayerState->GetPlayerLevel();
}

void AMainCharacter::EquipKatana()
{
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("Hand_RSocket"));
	if (HandSocket && Weapon)
	{
		if (HasAuthority())
		{
			Weapon->DetachFromCharacter();
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		else
		{
			ServerEquipRequest();
		}
	}	
}

void AMainCharacter::ServerEquipRequest_Implementation()
{
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("Hand_RSocket"));
	if (HandSocket && Weapon)
	{		
		Weapon->DetachFromCharacter();
		HandSocket->AttachActor(Weapon, GetMesh());
	}
}

void AMainCharacter::UnEquipKatana()
{
	if (BackAccessory)
	{
		const USkeletalMeshSocket* HandSocket = BackAccessory->GetSocketByName(FName("WeaponSocket"));
		if (HandSocket && Weapon)
		{
			if (HasAuthority())
			{
				Weapon->DetachFromCharacter();
				HandSocket->AttachActor(Weapon, BackAccessory);
			}
			else
			{
				ServerUnEquipRequest();
			}
		}
	}	
}

void AMainCharacter::ServerUnEquipRequest_Implementation()
{
	const USkeletalMeshSocket* HandSocket = BackAccessory->GetSocketByName(FName("WeaponSocket"));
	if (HandSocket && Weapon)
	{
		Weapon->DetachFromCharacter();
		HandSocket->AttachActor(Weapon, BackAccessory);
	}
}


FComboPackage AMainCharacter::GetElementArrComboPachage(int Index)
{
	if(CombatComponent)
	{		
		return CombatComponent->GetElementArrComboPachage(Index);
	}

	return FComboPackage();
}

void AMainCharacter::PlayNextCombo()
{
	if (CombatComponent)
	{
		CombatComponent->PlayNextCombo();
	}	
}

void AMainCharacter::ClearComboParams()
{
	if (CombatComponent)
	{
		CombatComponent->ClearSavingCombo();
		CombatComponent->ClearInputCombo();
		CombatComponent->SetbCanComboAttack(true);
		bCanJump = true;
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void AMainCharacter::SetbCanComboAttack(bool bCan)
{
	if (CombatComponent)
	{
		CombatComponent->SetbCanComboAttack(bCan);
	}
}

void AMainCharacter::Jump()
{
	if (bCanJump && (ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Walk || ActionState == EActionState::EAS_Run))
	{
		Super::Jump();
	}
}

void AMainCharacter::TurnInPlace(float Deltatime)
{
	if (AO_Yaw > 90.f)
	{
		if (ActionState == EActionState::EAS_Unoccupied)
		{
			StartingTurnRotation = FRotator(0.f, GetActorRotation().Yaw, 0.f);
			AO_Yaw = 0.f;		
			PlayTurnRightMontage();
		}
	}
	else if (AO_Yaw < -90.f)
	{
		if (ActionState == EActionState::EAS_Unoccupied)
		{
			StartingTurnRotation = FRotator(0.f, GetActorRotation().Yaw, 0.f);
			AO_Yaw = 0.f;
			PlayTurnLeftMontage();
		}
	}
}

void AMainCharacter::PollInit(float Deltatime)
{
	SetSpeed(UKismetMathLibrary::VSizeXY(GetVelocity()));
	bIsInAir = GetCharacterMovement()->IsFalling();
	WalkRunJump();

	if (MCSpeed == 0.f && !bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetActorForwardVector().Rotation().Yaw, 0.f);
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);	
		AO_Yaw = DeltaAimRotation.Yaw;		

		//TurnInPlace(Deltatime);

		return;
	}

	if(MCSpeed > 0.f || bIsInAir)
	{	
		SetMovementYaw(UKismetMathLibrary::NormalizedDeltaRotator(MoveVector.Rotation(), GetActorForwardVector().Rotation()).Yaw);

		AO_Yaw = 0.f;

		return;
	}
}

void AMainCharacter::CancelClimb()
{
	SetActionState(EActionState::EAS_CancelClimb);
	MPPlayerController->SetControlRotation(GetActorRotation());
}

void AMainCharacter::LedgeUpClimbing()
{
	SetActionState(EActionState::EAS_LedgeUpClimb);
}

void AMainCharacter::ClimbDashEnd()
{
	SetActionState(EActionState::EAS_Climb);
}

void AMainCharacter::WalkRunJump()
{
	if (ActionState > EActionState::EAS_Jump || ActionState == EActionState::EAS_Climb) return;

	if (bIsInAir)
	{
		SetActionState(EActionState::EAS_Jump);
	}
	else if (!bIsInAir && MCSpeed > 0.f && MCSpeed < 100.f)
	{		
		SetActionState(EActionState::EAS_Walk);
	}
	else if (!bIsInAir && MCSpeed >= 100.f)
	{		
		SetActionState(EActionState::EAS_Run);
	}
	else if (!bIsInAir && MCSpeed == 0.f && ActionState != EActionState::EAS_CancelClimb)
	{
		SetActionState(EActionState::EAS_Unoccupied);
	}
}

void AMainCharacter::PlayTurnRightMontage()
{
	if (TurnRight)
	{
		PlayAnimMontage(TurnRight);
	}
}

void AMainCharacter::PlayTurnLeftMontage()
{
	if (TurnLeft)
	{
		PlayAnimMontage(TurnLeft);
	}
}

void AMainCharacter::ServerRequestSetActionState_Implementation(EActionState Action)
{
	ActionState = Action;
}

void AMainCharacter::SetActionState(EActionState Action)
{
	if (HasAuthority())
		ActionState = Action;
	else
		ServerRequestSetActionState(Action);
}

void AMainCharacter::SetMoveVector(FVector InVector)
{
	if (HasAuthority())
		MoveVector = InVector;
	else
		ServerRequestSetMoveVecctor(InVector);
}

void AMainCharacter::ServerRequestSetMoveVecctor_Implementation(FVector InVector)
{
	MoveVector = InVector;
}

void AMainCharacter::ServerSetMovementYaw_Implementation(float InYaw)
{
	Movement_Yaw = InYaw;
}

void AMainCharacter::SetMovementYaw(float InYaw)
{
	if (HasAuthority())
		Movement_Yaw = InYaw;
	else
		ServerSetMovementYaw(InYaw);
}

void AMainCharacter::ServerSetSpeed_Implementation(float InSpeed)
{
	MCSpeed = InSpeed;
}

void AMainCharacter::SetSpeed(float InSpeed)
{
	if (HasAuthority())
		MCSpeed = InSpeed;
	else
		ServerSetSpeed(InSpeed);
}

void AMainCharacter::SetControllerRotationAndDesiredRotation(const bool bTurnOn)
{
	MovementComponent->bUseControllerDesiredRotation = bTurnOn;
	bUseControllerRotationYaw = bTurnOn;
}

void AMainCharacter::SetWeaponBoxCollisionEnabled()
{
	Weapon->SetBoxCollisionEnabled();
}

void AMainCharacter::SetWeaponBoxCollisionDisabled()
{
	Weapon->SetBoxCollisionDisabled();
	Weapon->SetIgnoreActorClear();
}
