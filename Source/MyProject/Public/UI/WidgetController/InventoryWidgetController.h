// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/MPWidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FMPInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdateSignature, const TArray<FMPInventoryItem>&, ItemArray);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MYPROJECT_API UInventoryWidgetController : public UMPWidgetController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeController();

	UFUNCTION(BlueprintCallable)
	void DragMoveItem(int32 InNewInventoryNumber);

	UFUNCTION(BlueprintCallable)
	void UseItem(int32 InventoryNumber);

	UFUNCTION(BlueprintCallable)
	const TArray< FMPInventoryItem> GetInventoryItemList(AActor* TargetActor);
	
	UPROPERTY(BlueprintAssignable)
	FInventoryUpdateSignature InventoryUpdateDelegate;

private:
	int32 DraggedSlotIndex;
	bool bDraggedExchanged = false;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetDraggedSlotIndex() { return DraggedSlotIndex; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDraggedSlotIndex(int32 Index) { DraggedSlotIndex = Index; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetDraggedExchanged() { return bDraggedExchanged; }

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetDraggedExchanged(bool bChaged) {	bDraggedExchanged = bChaged;	}


};
