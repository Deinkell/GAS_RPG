// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/MPWidgetController.h"
#include "GameplayTagContainer.h"
#include "MPGameplayTags.h"
#include "MPAbilityTypes.h"

#include "SpellMenuWidgetController.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPreventForError, FString, DescriptionString, FString, NextLevelDescriptionString);


struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MYPROJECT_API USpellMenuWidgetController : public UMPWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReAssignedSignature SpellGlobeReassignedDelegate;

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

private:
	static void ShouldEnalbeButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnalbeEquipButton);

private:
	FSelectedAbility SelectedAbility = { FMPGameplayTags::Get().Abilities_Type_None, FMPGameplayTags::Get().Abilities_Status_Locked };
	int32 CurrentSpellPoints = 0;

	bool bWaitingForEquipSelection = false;

	FGameplayTag SelectedSlot;
};
