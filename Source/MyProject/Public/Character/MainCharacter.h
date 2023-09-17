// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "InputActionValue.h"
#include "BaseCharacter.h"
#include "Interaction/PlayerInterface.h"
\
#include "MainCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FComboPackage;
class UMainCharacterMovementComponent;
class UNiagaraComponent;


UCLASS()
class MYPROJECT_API AMainCharacter : public ABaseCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:	
	AMainCharacter(const FObjectInitializer& ObjectInitializer);
	void Initialize();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool CanCastFirebolt() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetAllowPhysicsRotationDuringRootMotion(bool bAllow);

	/* PlayerInterface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetAttributePointReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointReward_Implementation(int32 Level) const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;

	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void EquipKatana();

	UFUNCTION(BlueprintCallable)
	void UnEquipKatana();						

	UFUNCTION(BlueprintCallable)
	FComboPackage GetElementArrComboPachage(int Index);

	UFUNCTION(BlueprintCallable)
	void PlayNextCombo();	

	UFUNCTION(BlueprintCallable)
	void ClearComboParams();

	UFUNCTION(BlueprintCallable)
	void SetbCanComboAttack(bool bCan);

	virtual void Jump() override;
	void TurnInPlace(float Deltatime);	
	void PollInit(float Deltatime);

	UFUNCTION()
	void CancelClimb();

	UFUNCTION()
	void LedgeUpClimbing();

	UFUNCTION()
	void ClimbDashEnd();

	void WalkRunJump();
	void PlayTurnRightMontage();
	void PlayTurnLeftMontage();

	UFUNCTION(BlueprintCallable)
	void SetActionState(EActionState Action);

	UFUNCTION(Server, Reliable)
	void ServerRequestSetActionState(EActionState Action);

	void SetMoveVector(FVector InVector);

	UFUNCTION(Server, Reliable)
	void ServerRequestSetMoveVecctor(FVector InVector);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

	UFUNCTION(BlueprintCallable)
	void SetControllerRotationAndDesiredRotation(const bool bTurnOn);

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollisionEnabled();

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollisionDisabled();

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(Server, Reliable)
	void ServerEquipRequest();

	UFUNCTION(Server, Reliable)
	void ServerUnEquipRequest();

	UFUNCTION(Server, Reliable)
	void ServerSetMovementYaw(float InYaw);

	void SetMovementYaw(float InYaw);

	UFUNCTION(Server, Reliable)
	void ServerSetSpeed(float InSpeed);

	void SetSpeed(float InSpeed);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Character)
	UMainCharacterMovementComponent* MovementComponent;

public:
	bool bCanJump = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private: //BaseComponent(Camera, Mesh, Weapon)
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
	class AMainCharacterPlayerController* MPPlayerController;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Gloves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* BodyUpper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* ShoulderPad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* BodyLower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Foot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* HeadAccessory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Clothes, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* BackAccessory;

private: //Turning Montage
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	class UAnimMontage* TurnLeft;

	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	class UAnimMontage* TurnRight;

private: //CombatComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	class UCombatComponent* CombatComponent;

private: //etc, var
	UPROPERTY(VisibleAnywhere, Replicated, Category = Combat)
	ECharacterState CharacterState;

	UPROPERTY(VisibleAnywhere, Replicated, Category = Movement)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY()
	FRotator StartingAimRotation;

	UPROPERTY()
	FRotator StartingTurnRotation;

	UPROPERTY(Replicated)
	FVector MoveVector;

	UPROPERTY(Replicated)
	float Movement_Yaw;

	UPROPERTY(Replicated)
	float MCSpeed;

	UPROPERTY(Replicated)
	bool bIsInAir;

	UPROPERTY(Replicated)
	float AO_Yaw;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCharacterState(ECharacterState State) { CharacterState = State; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UMainCharacterMovementComponent* GetCustomMovementComponent() const { return MovementComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AMainCharacterPlayerController* GetMPPlayerController() { return MPPlayerController; }

	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE FVector GetMoveVector() const { return MoveVector; }
	FORCEINLINE bool CanRun() const { return !(ActionState > EActionState::EAS_Run); }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
	FORCEINLINE float GetSpeed() const { return MCSpeed; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetMovementYaw() const { return Movement_Yaw;  }
	FORCEINLINE void SetPlayerController(AMainCharacterPlayerController* InPlayerController) { MPPlayerController = InPlayerController; }
};
