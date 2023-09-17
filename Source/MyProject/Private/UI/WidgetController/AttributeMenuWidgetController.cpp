// EvoAnd Copyright


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/MPAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "MPGameplayTags.h"
#include "GameplayTagContainer.h"
#include "Game/MPPlayerState.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UMPAttributeSet* AS = CastChecked<UMPAttributeSet>(AttributeSet);

	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	MPPlayerState->OnAttributePointsChangedDelegate.Broadcast(GetMPPS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	for (auto& Pair : GetMPAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s : %f"), *Pair.Key.ToString(), Pair.Value().GetNumericValue(AttributeSet));
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	GetMPPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangeDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UMPAbilitySystemComponent* MPASC = CastChecked<UMPAbilitySystemComponent>(AbilitySystemComponent);
	MPASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FMPAttributeInfo Info = AttributeInfo->FindAttributeInfo(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
