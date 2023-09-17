// EvoAnd Copyright


#include "Component/InventoryComponent.h"
#include "Actor/MPEffectActor.h"
#include "MPAbilitySystemBlueprintLibrary.h"
#include "Actor/InveontoryMPEffectActor.h"



UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	for (int i = 0; i < NumberOfInventory; i++)
	{
		InventoryNumberArr.Add(false);
	}
}

void UInventoryComponent::SetInventoryItem_Implementation(FMPInventoryItem& InItem)
{
	bool IsSameItem = false;
	for (auto& Item : InventoryItemList)
	{
		if (Item.ItemType == InItem.ItemType)
		{
			Item.ItemNum += 1;
			IsSameItem = true;
			break;
		}
	}

	for (int i = 0; i < NumberOfInventory; i++)
	{
		if (IsSameItem) break;

		if (!InventoryNumberArr[i])
		{
			InventoryNumberArr[i] = true;
			InItem.InventoryNum = i;
			break;
		}
	}

	if(!IsSameItem) InventoryItemList.AddUnique(InItem);

	InventoryItmeListChangedDelegate.Broadcast(InventoryItemList);
}

void UInventoryComponent::UseInventoryItem(int32 InventoryNum)
{
	if (InventoryItemList.Num() <= 0) return;

	bool bCanUseItem = false;
	FMPInventoryItem InventoryItem;
	for (auto& Item : InventoryItemList)
	{
		if (Item.InventoryNum == InventoryNum)
		{
			InventoryItem = Item;
			bCanUseItem = true;

			if (InventoryItem.ItemNum == 1)
			{			
				InventoryNumberArr[InventoryItem.InventoryNum] = false;
				InventoryItemList.RemoveSingle(Item);
				break;
			}
			else
			{			
				Item.ItemNum -= 1;				
				break;
			}
		}
	}

	if (bCanUseItem && InventoryOwner)
	{
		UMPAbilitySystemBlueprintLibrary::ApplyEffectToMainCharacter(InventoryOwner, InventoryItem.InstantGameplayEffectClass, InventoryItem.ActorLevel);
		InventoryItmeListChangedDelegate.Broadcast(InventoryItemList);
	}
}

void UInventoryComponent::ExechangeInventoryNumber(int32 InDraggedIndex, int32 NewSlotIndex)
{
	FMPInventoryItem* DraggedItem = nullptr, *NewSlotItem = nullptr;
	for (auto& Slot : InventoryItemList)
	{
		if (Slot.InventoryNum == InDraggedIndex)
			DraggedItem = &Slot;

		if(Slot.InventoryNum == NewSlotIndex)
			NewSlotItem = &Slot;
	}

	if (DraggedItem && NewSlotItem)
	{
		int32 Temp = DraggedItem->InventoryNum;
		DraggedItem->InventoryNum = NewSlotItem->InventoryNum;
		NewSlotItem->InventoryNum = Temp;

		return;
	}

	if (DraggedItem && NewSlotItem == nullptr)
	{
		DraggedItem->InventoryNum = NewSlotIndex;
	}
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}




