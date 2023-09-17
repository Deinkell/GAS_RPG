// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Character/CharacterTypes.h"
#include "Components/CapsuleComponent.h"
#include "Character/MainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UMainCharacterMovementComponent::UMainCharacterMovementComponent()
{	
	bUseControllerDesiredRotation = false;
}

void UMainCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UMainCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	SweepAndStoreWallHits();
}

bool UMainCharacterMovementComponent::TryClimbing()
{	
	if (CanStartClimbing())
	{		
		bWantsToClimb = true;	
		return true;
	}

	return false;
}

void UMainCharacterMovementComponent::CancelClimbing()
{
	bWantsToClimb = false;
}

bool UMainCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::ECMOVE_Climbing;
}

FVector UMainCharacterMovementComponent::GetClimbSurfaceNormal() const
{
	return CurrentClimbingNormal;
}

void UMainCharacterMovementComponent::TryClimbDashing()
{
	if (ClimbDashCurve && bWantsToClimbDash == false)
	{
		bWantsToClimbDash = true;
		CurrentClimbDashTime = 0.f;
		StoreClimbDashDirection();
	}
}

void UMainCharacterMovementComponent::StoreClimbDashDirection()
{
	ClimbDashDirection = UpdatedComponent->GetUpVector();
	const float AccelerationThreshold = MaxClimbingAcceleration / 10.f;
	if (Acceleration.Length() > AccelerationThreshold)
	{
		ClimbDashDirection = Acceleration.GetSafeNormal();
	}
}

void UMainCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if(bWantsToClimb)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::ECMOVE_Climbing);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UMainCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::ECMOVE_Climbing)
	{
		PhysClimbing(DeltaTime, Iterations);
	}

	Super::PhysCustom(DeltaTime, Iterations);
}

void UMainCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{		
		OwnerMainCharacter->SetControllerRotationAndDesiredRotation(false);
		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECMOVE_Climbing;
	if (bWasClimbing)
	{		
		OwnerMainCharacter->SetControllerRotationAndDesiredRotation(true);
		const FRotator StandRotation = FRotator(0., UpdatedComponent->GetComponentRotation().Yaw, 0.f);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

float UMainCharacterMovementComponent::GetMaxSpeed() const
{
	return IsClimbing() ? MaxClimbingSpeed : Super::GetMaxSpeed();
}

float UMainCharacterMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

void UMainCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	ComputeSurfaceInfo();

	if (ShouldStopClimbing() || ClimbDownToFloor())
	{
		StopClimbing(deltaTime, Iterations);		
		if (OwnerMainCharacter->GetActionState() != EActionState::EAS_LedgeUpClimb)
		{
			OnCancelClimbDelegate.ExecuteIfBound();
		}

		return;
	}
	UpdateClimbDashState(deltaTime);
	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	MoveAlongClimbingSurface(deltaTime);
	TryClimbUpLedge();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	SnapToClimbingSurface(deltaTime);
}

void UMainCharacterMovementComponent::ComputeSurfaceInfo()
{
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;

	if (CurrentWallHits.IsEmpty())
	{
		return;
	}

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(6);

	for (const FHitResult& WallHit : CurrentWallHits)
	{
		const FVector End = Start + (WallHit.ImpactPoint - Start).GetSafeNormal() * 120;

		FHitResult AssistHit;
		GetWorld()->SweepSingleByChannel(AssistHit, Start, End, FQuat::Identity, ECC_WorldStatic, CollisionSphere, ClimbQueryParams);

		CurrentClimbingPosition += AssistHit.ImpactPoint;
		CurrentClimbingNormal += AssistHit.Normal;
	}

	CurrentClimbingPosition /= CurrentWallHits.Num();
	CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

void UMainCharacterMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		if (bWantsToClimbDash)
		{
			AlignClimbDashDirection();

			const float CurrentCurveSpeed = ClimbDashCurve->GetFloatValue(CurrentClimbDashTime);
			Velocity = ClimbDashDirection * CurrentCurveSpeed;
		}
		else
		{
			constexpr float Friction = 0.f;
			constexpr bool bFluid = false;
			CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
		}
	}

	ApplyRootMotionToVelocity(deltaTime);
}

bool UMainCharacterMovementComponent::ShouldStopClimbing() const
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	return !bWantsToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UMainCharacterMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	StopClimbDashing();
	bWantsToClimb = false;
	SetMovementMode(EMovementMode::MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
}

void UMainCharacterMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UMainCharacterMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);

	constexpr bool bSweep = true;
	const float SnapSpeed = ClimbingSnapSpeed * FMath::Max(1, Velocity.Length() / MaxClimbingSpeed);
	UpdatedComponent->MoveComponent(Offset * SnapSpeed * deltaTime, Rotation, bSweep);
}

void UMainCharacterMovementComponent::SweepAndStoreWallHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	HitWall ? CurrentWallHits = Hits : CurrentWallHits.Reset();
	
	/**Debug**/
/*	for (auto& Hit : CurrentWallHits)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 5, 10, FColor::Blue);
	}*/
	/****/
}

bool UMainCharacterMovementComponent::CanStartClimbing() 
{
	for (FHitResult& Hit : CurrentWallHits)
	{		
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();
		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);
		const float HorizontalDegree = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));
		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		bool bCanStartClimbing = HorizontalDegree <= MinHorizontalDegreesToStartClimbing && !bIsCeiling && IsFacingSurface(VerticalDot);
		if (bCanStartClimbing)
		{
			return true;
		}
	}

	return false;
}

bool UMainCharacterMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult HitResult;

	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + ClimbingCollisionShrinkAmount : BaseEyeHeight;

	const FVector Start = UpdatedComponent->GetComponentLocation() + (UpdatedComponent->GetUpVector() * EyeHeightOffset);
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);

	const bool bEyeHeightTrace = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, ClimbQueryParams);
	
	/**Debug**/
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red);	

	//if (HitResult.bBlockingHit)
	//{
	//	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5, 10, FColor::Cyan);
	//}
	/****/

	return bEyeHeightTrace;
}

bool UMainCharacterMovementComponent::IsFacingSurface(const float SurfaceVericalDot) const
{
	constexpr float BaseLength = 80.f;
	const float SteepnessMultiplier = 1 + (1 - SurfaceVericalDot) * 5;

	return EyeHeightTrace(BaseLength  * SteepnessMultiplier);
}

FQuat UMainCharacterMovementComponent::GetClimbingRotation(float deltaTime) const
{
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) return Current;

	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();
	const float RotationSpeed = ClimbingRotationSpeed * FMath::Max(1, Velocity.Length() / MaxClimbingSpeed);

	return FMath::QInterpTo(Current, Target, deltaTime, RotationSpeed);
}

bool UMainCharacterMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;
	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();
	
	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;

	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();

	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UMainCharacterMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Yellow);

	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECollisionChannel::ECC_WorldStatic, ClimbQueryParams) ;
}

bool UMainCharacterMovementComponent::TryClimbUpLedge() const
{
	if (AnimInstance && LedgeClimbMontage && AnimInstance->Montage_IsPlaying(LedgeClimbMontage))
	{
		return false;
	}

	const float UpSpeed = FVector::DotProduct(Velocity, UpdatedComponent->GetUpVector());
	const bool bIsMovingUp = UpSpeed >= MaxClimbingSpeed / 3;

	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation())
	{
		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);
		AnimInstance->Montage_Play(LedgeClimbMontage);
		OnLedgeUpClimbDelegate.ExecuteIfBound();

		return true;
	}

	return false;
}

bool UMainCharacterMovementComponent::HasReachedEdge() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 2.5f;

	return !EyeHeightTrace(TraceDistance);
}

bool UMainCharacterMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 250.f);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd, ECC_WorldStatic, ClimbQueryParams);

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

bool UMainCharacterMovementComponent::CanMoveToLedgeClimbLocation() const
{
	const FVector VerticalOffset = FVector::UpVector * 160.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 120.f;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;
	
	if (!IsLocationWalkable(CheckLocation)) return false;

	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck, CheckLocation, FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);

	return !bBlocked;
}

void UMainCharacterMovementComponent::UpdateClimbDashState(float DeltaTime)
{
	if (!bWantsToClimbDash) return;

	CurrentClimbDashTime += DeltaTime;

	float MinTime, MaxTime;
	ClimbDashCurve->GetTimeRange(MinTime, MaxTime);

	if (CurrentClimbDashTime >= MaxTime)
	{
		StopClimbDashing();
	}
}

void UMainCharacterMovementComponent::StopClimbDashing()
{
	bWantsToClimbDash = false;
	CurrentClimbDashTime = 0.f;
	OnClimbDashDelegate.ExecuteIfBound();
}

void UMainCharacterMovementComponent::AlignClimbDashDirection()
{
	const FVector HorizontalSurfaceNormal = GetClimbSurfaceNormal().GetSafeNormal2D();
	ClimbDashDirection = FVector::VectorPlaneProject(ClimbDashDirection, HorizontalSurfaceNormal);
}
