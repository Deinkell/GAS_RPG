// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"

#include "MainCharacterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AMainCharacter;
struct FInputActionValue;
class IEnemyInterface;
class UMPInputConfig;
class UMPAbilitySystemComponent;
class USplineComponent;
class UDamageTextComponent;

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMainCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainCharacterPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockHit, bool bCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	void Climb();
	void InputClimb();

	UFUNCTION(Server, Reliable)
	void ServerClimb();

	void CancelClimb();
	void InputCancelClimb();

	UFUNCTION(Server, Reliable)
	void ServerCancelClimb();

	void Jump();
	void EquipUnEquip();

	void BasicComboAttack(const FInputActionValue& Value);
	void InputBasicComboAttack(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerBasicComboAttack(const FInputActionValue& Value);

	void ClimbDash();
	void CursorTrace();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UMPAbilitySystemComponent* GetASC();
	void AutoRun();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> MainCharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> ClimbAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> CancelClimbAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> BasicComboAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> ClimbDashing;

	UPROPERTY()
	TObjectPtr<AMainCharacter> OwnerMainCharacter;	

	UPROPERTY()
	TObjectPtr<class UMainCharacterMovementComponent> OwnerMainCharacterMovementComponent;

	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UMPInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr< UMPAbilitySystemComponent> MPAbilitySystemComponent;
		
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	bool bShiftKeyDown = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent>Spline;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

public:
	void SetOwnerMainCharacter(AMainCharacter* InOwner);

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
};
