// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/InventoryInterface.h"

#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryItemListChanged, const TArray<FMPInventoryItem>& /* ItemArray */);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	virtual void SetInventoryItem_Implementation(FMPInventoryItem& InItem) override; // ���� �����ϱ� ���� �ݵ�� InventoryNum�� ����������Ѵ�

	UFUNCTION()
	void UseInventoryItem(int32 InventoryNum);
	void ExechangeInventoryNumber(int32 InDraggedIndex, int32 NewSlotIndex);

	FOnInventoryItemListChanged InventoryItmeListChangedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumberOfInventory = 30;

protected:
	virtual void BeginPlay() override;

private:
	TArray<FMPInventoryItem> InventoryItemList;
	AActor* InventoryOwner;

	TArray<bool> InventoryNumberArr;

public:
	FORCEINLINE void SetInventoryOwner(AActor* InOwner) { InventoryOwner = InOwner; }
	FORCEINLINE AActor* GetInventoryOwner() { return InventoryOwner; }
	FORCEINLINE const TArray< FMPInventoryItem>* GetInventoryItemList() { return &InventoryItemList;	}
		
};
