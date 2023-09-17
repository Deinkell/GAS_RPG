// EvoAnd Copyright


#include "UI/WidgetController/MPWidgetController.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "AbilitySystem/MPAttributeSet.h"
#include "Game/MPPlayerState.h"
#include "PlayerController/MainCharacterPlayerController.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UMPWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UMPWidgetController::BroadcastInitialValues()
{
}

void UMPWidgetController::BindCallbacksToDependencies()
{
}

void UMPWidgetController::BroadcastAbilityInfo()
{
	if (!GetMPASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FMPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(MPAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = MPAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = MPAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		});

	GetMPASC()->ForEachAbility(BroadcastDelegate);
}

AMainCharacterPlayerController* UMPWidgetController::GetMPPC()
{
	if (MPPlayerController == nullptr)
	{
		MPPlayerController = Cast<AMainCharacterPlayerController>(PlayerController);
	}

	return MPPlayerController;
}

AMPPlayerState* UMPWidgetController::GetMPPS()
{
	if (MPPlayerState == nullptr)
	{
		MPPlayerState = Cast<AMPPlayerState>(PlayerState);
	}

	return MPPlayerState;
}

UMPAbilitySystemComponent* UMPWidgetController::GetMPASC()
{
	if (MPAbilitySystemComponent == nullptr)
	{
		MPAbilitySystemComponent = Cast<UMPAbilitySystemComponent>(AbilitySystemComponent);
	}

	return MPAbilitySystemComponent;
}

UMPAttributeSet* UMPWidgetController::GetMPAS()
{
	if (MPAttributeSet == nullptr)
	{
		MPAttributeSet = Cast<UMPAttributeSet>(AttributeSet);
	}

	return MPAttributeSet;
}
