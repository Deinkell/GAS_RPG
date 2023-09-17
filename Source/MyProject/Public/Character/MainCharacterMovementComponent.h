// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacterMovementComponent.generated.h"

DECLARE_DELEGATE(FOnNoticeClimbState);

UCLASS()
class MYPROJECT_API UMainCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UMainCharacterMovementComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool TryClimbing();
	void CancelClimbing();

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

	UFUNCTION(BlueprintCallable)
	void TryClimbDashing();

	UFUNCTION(BlueprintPure)
	bool IsClimbDashing() const
	{
		return IsClimbing() && bWantsToClimbDash;
	}

	UFUNCTION(BlueprintPure)
	FVector GetClimbDashdirection() const
	{
		return ClimbDashDirection;
	}

protected:
	virtual void BeginPlay() override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity);

private:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	void PhysClimbing(float deltaTime, int32 Iterations);
	void ComputeSurfaceInfo();
	void ComputeClimbingVelocity(float deltaTime);
	bool ShouldStopClimbing() const;
	void StopClimbing(float deltaTime, int32 Iterations);
	void MoveAlongClimbingSurface(float deltaTime);
	void SnapToClimbingSurface(float deltaTime) const;
	void SweepAndStoreWallHits();
	bool CanStartClimbing();
	bool EyeHeightTrace(const float TraceDistance) const;
	bool IsFacingSurface(const float SurfaceVericalDot) const;
	FQuat GetClimbingRotation(float deltaTime) const;
	bool ClimbDownToFloor() const;
	bool CheckFloor(FHitResult& FloorHit) const;
	bool TryClimbUpLedge() const;

	bool HasReachedEdge() const;
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	bool CanMoveToLedgeClimbLocation() const;

	void StoreClimbDashDirection();
	void UpdateClimbDashState(float DeltaTime);
	void StopClimbDashing();
	void AlignClimbDashDirection();

public:
	FOnNoticeClimbState OnCancelClimbDelegate;
	FOnNoticeClimbState OnLedgeUpClimbDelegate;
	FOnNoticeClimbState OnClimbDashDelegate;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = true))
	int CollisionCapsuleRadius = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = true))
	int CollisionCapsuleHalfHeight = 72;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Climbing", meta = (AllowPrivateAccess = true))
	TArray<FHitResult> CurrentWallHits;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "1.0", ClampMax = "75.0"))
	float MinHorizontalDegreesToStartClimbing = 25.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float ClimbingCollisionShrinkAmount = 30.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxClimbingSpeed = 120.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "10.0", ClampMax = "2000.0"))
	float MaxClimbingAcceleration = 380.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float BrakingDecelerationClimbing = 550.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "1.0", ClampMax = "12.0"))
	int32 ClimbingRotationSpeed = 6;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "0.0", ClampMax = "60.0"))
	float ClimbingSnapSpeed = 4.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float DistanceFromSurface = 45.f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Climbing", meta = (ClampMin = "1.0", ClampMax = "500.0"))
	float FloorCheckDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement : Climbing")
	UAnimMontage* LedgeClimbMontage;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY()
	TObjectPtr<class AMainCharacter> OwnerMainCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement : Climbing")
	UCurveFloat* ClimbDashCurve;

	FCollisionQueryParams ClimbQueryParams;
	FVector CurrentClimbingNormal;
	FVector CurrentClimbingPosition;
	bool bWantsToClimb = false;
	FVector ClimbDashDirection;
	bool bWantsToClimbDash = false;
	float CurrentClimbDashTime;

public:
	FORCEINLINE void SetOwnerMainCharacter(AMainCharacter* InOwner) { OwnerMainCharacter = InOwner; }
};
