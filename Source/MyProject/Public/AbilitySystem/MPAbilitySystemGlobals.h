// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "MPAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;	
};
