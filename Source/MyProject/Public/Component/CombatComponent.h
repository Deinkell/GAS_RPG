// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes.h"

#include "CombatComponent.generated.h"

USTRUCT(BlueprintType)
struct FComboPackage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EComboState ComboState;

	UPROPERTY(EditAnywhere)
	int32 Combo_Num;

	UPROPERTY(EditAnywhere)
	FName SectionName;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FName LinkNextComboState();	
	void SetNextComboState();
	void PlayNextCombo();

	FComboPackage GetElementArrComboPachage(int Index);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AMainCharacter* OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Katana, meta = (AllowPrivateAccess = true))
	UAnimMontage* KatanaComboMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = true))
	TArray<FComboPackage> ArrComboPackage;	

	bool bCanComboAttack = true;

private:
	FComboPackage SavingCombo;
	FComboPackage InputCombo;

public:
	FORCEINLINE void ClearSavingCombo() { SavingCombo = FComboPackage(); }
	FORCEINLINE void ClearInputCombo() { InputCombo = FComboPackage(); }
	FORCEINLINE void SetbCanComboAttack(bool bCan) { bCanComboAttack = bCan; }
	FORCEINLINE bool GetbCanComboAttack() const { return bCanComboAttack; }
	FORCEINLINE UAnimMontage* GetKatanaMontage() const { return KatanaComboMontage; }
	FORCEINLINE void SetOwnerCharacter(AMainCharacter* InOwner) { OwnerCharacter = InOwner; }
};
