// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Actor/MPEffectActor.h"

#include "InveontoryMPEffectActor.generated.h"

UENUM(BlueprintType)
enum class EItemType
{
	HealthPotion,
	ManaPotion,
	HealthCrystal,
	ManaCrystal,
	None
};

USTRUCT(BlueprintType, Blueprintable)
struct FMPInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemNum = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMPEffectActor> ItemTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 InventoryNum = -1;

	int32 ActorLevel = 1;

	bool operator== (const FMPInventoryItem& InItem) const;
};



/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MYPROJECT_API AInveontoryMPEffectActor : public AMPEffectActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void OnOverlapToSetInventory(AActor* TargetActor);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
	FMPInventoryItem ItemData;
};
