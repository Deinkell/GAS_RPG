// EvoAnd Copyright


#include "Actor/InveontoryMPEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/BaseCharacter.h"

/* FMPInventoryItem */
bool FMPInventoryItem::operator==(const FMPInventoryItem& InItem) const
{
	if (ItemType == InItem.ItemType)
		return true;

	return false;
}

/* AInveontoryMPEffectActor */
void AInveontoryMPEffectActor::BeginPlay()
{
	Super::BeginPlay();

}

void AInveontoryMPEffectActor::OnOverlapToSetInventory(AActor* TargetActor)
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(TargetActor))
	{
		Character->AddInventoryItem(ItemData, -1);
	}

	Destroy();
}

