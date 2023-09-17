// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes.h"

#include "MainCharacterAnimInstance.generated.h"

UCLASS()
class MYPROJECT_API UMainCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetMainCharacterAllowPhysicsRotationDuringRootMotion(bool bAllow);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	class AMainCharacter* MainCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	TObjectPtr<class UMainCharacterMovementComponent> MainCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	float Movement_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	FVector UnRotatedVelocity;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	bool IsClimbingDash =false;

	UPROPERTY(BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess))
	FVector2D ClimbDashVelocity;
};
