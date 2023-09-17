// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/MainCharacterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MainCharacterMovementComponent.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	if (MainCharacter)
	{
		MainCharacterMovement = Cast<UMainCharacterMovementComponent>(MainCharacter->GetMovementComponent());
	}
}

void UMainCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MainCharacter == nullptr) return;
	
	GroundSpeed = MainCharacter->GetSpeed();
	IsFalling = MainCharacterMovement->IsFalling();
	CharacterState = MainCharacter->GetCharacterState();
	ActionState = MainCharacter->GetActionState();
	DeathPose = MainCharacter->GetDeathPose();
	AO_Yaw = MainCharacter->GetAO_Yaw();
	Movement_Yaw = MainCharacter->GetMovementYaw();
	UnRotatedVelocity = UKismetMathLibrary::Quat_UnrotateVector(MainCharacter->GetActorRotation().Quaternion(), MainCharacter->GetVelocity());	
	IsClimbingDash = MainCharacterMovement->IsClimbDashing();
	FVector UnRotatedClimbingDash = UKismetMathLibrary::Quat_UnrotateVector(MainCharacter->GetActorRotation().Quaternion(), MainCharacterMovement->GetClimbDashdirection());
	ClimbDashVelocity = FVector2D(UnRotatedClimbingDash.Y, UnRotatedClimbingDash.Z);	
}

void UMainCharacterAnimInstance::SetMainCharacterAllowPhysicsRotationDuringRootMotion(bool bAllow)
{
	if (MainCharacter)
	{
		MainCharacter->SetAllowPhysicsRotationDuringRootMotion(bAllow);
	}
}


