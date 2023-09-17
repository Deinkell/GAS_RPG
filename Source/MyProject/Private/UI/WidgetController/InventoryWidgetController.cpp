// EvoAnd Copyright


#include "UI/WidgetController/InventoryWidgetController.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "Character/BaseCharacter.h"
#include "Component/InventoryComponent.h"

void UInventoryWidgetController::InitializeController()
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetMPASC()->GetAvatarActor()))
	{
		BaseCharacter->GetInventory()->InventoryItmeListChangedDelegate.AddLambda(
			[this](const TArray<FMPInventoryItem>& ItemArray)
			{
				InventoryUpdateDelegate.Broadcast(ItemArray);
			}
		);
	}
}

void UInventoryWidgetController::DragMoveItem(int32 InNewInventoryNumber)
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetMPASC()->GetAvatarActor()))
	{
		BaseCharacter->GetInventory()->ExechangeInventoryNumber(DraggedSlotIndex, InNewInventoryNumber);
		InventoryUpdateDelegate.Broadcast(*BaseCharacter->GetInventory()->GetInventoryItemList());
	}

}

void UInventoryWidgetController::UseItem(int32 InventoryNumber)
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetMPASC()->GetAvatarActor()))
	{
		BaseCharacter->GetInventory()->UseInventoryItem(InventoryNumber);
		InventoryUpdateDelegate.Broadcast(*BaseCharacter->GetInventory()->GetInventoryItemList());
	}	
}

const TArray< FMPInventoryItem> UInventoryWidgetController::GetInventoryItemList(AActor* TargetActor)
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(TargetActor))
	{
		return *BaseCharacter->GetInventory()->GetInventoryItemList();
	}

	return TArray< FMPInventoryItem>();
}
