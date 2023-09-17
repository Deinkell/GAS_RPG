// EvoAnd Copyright


#include "AbilitySystem/MPAbilitySystemGlobals.h"
#include "MPAbilityTypes.h"

FGameplayEffectContext* UMPAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMPGameplayEffectContext();
}
