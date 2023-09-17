// EvoAnd Copyright


#include "Input/MPInputConfig.h"
#include "InputAction.h"

const UInputAction* UMPInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (auto& Action : AbilityInputAction)
	{
		if (Action.InputAction &&  InputTag.MatchesTagExact(Action.InputTag))
		{
			return Action.InputAction;
		}
	}

	if (!bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s], on InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
