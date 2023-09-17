// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "Character/MainCharacter.h"
#include "AnimInstance/MainCharacterAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SavingCombo.ComboState = EComboState::ECS_NoCombo;
	SavingCombo.Combo_Num = 0;
	SavingCombo.SectionName = TEXT("");

	InputCombo.ComboState = EComboState::ECS_NoCombo;
	InputCombo.Combo_Num = 0;
	InputCombo.SectionName = TEXT("");
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FComboPackage UCombatComponent::GetElementArrComboPachage(int Index)
{
	if (ArrComboPackage.Num() <= Index) return FComboPackage();

	return ArrComboPackage[Index];
}

void UCombatComponent::SetNextComboState()
{	
	ClearInputCombo();

	if (SavingCombo.Combo_Num < ArrComboPackage.Num())
	{
		InputCombo = ArrComboPackage[SavingCombo.Combo_Num];
	}
	else
	{
		InputCombo = FComboPackage();
	}
}

void UCombatComponent::PlayNextCombo()
{
	if (OwnerCharacter && OwnerCharacter->GetCharacterState() == ECharacterState::ECS_KatanaCombat && OwnerCharacter->GetActionState() <= EActionState::EAS_Jump)
	{
		FName SectionName = LinkNextComboState();
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (SectionName != FName("") && bCanComboAttack && AnimInstance)
		{
			AnimInstance->Montage_Play(KatanaComboMontage);
			AnimInstance->Montage_JumpToSection(SectionName, KatanaComboMontage);
			bCanComboAttack = false;
		}
	}
}

FName UCombatComponent::LinkNextComboState()
{
	SavingCombo.SectionName = FName("");
	UMainCharacterAnimInstance* AnimInstance = Cast<UMainCharacterAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	
	if (!bCanComboAttack || ArrComboPackage.Num() == 0 || SavingCombo.Combo_Num == InputCombo.Combo_Num) return SavingCombo.SectionName;	

	SavingCombo = InputCombo;	

	return SavingCombo.SectionName;
}


