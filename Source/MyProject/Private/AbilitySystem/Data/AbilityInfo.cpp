// EvoAnd Copyright


#include "AbilitySystem/Data/AbilityInfo.h"
#include "MPLogChannel.h"

FMPAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FMPAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if (bLogNotFound) UE_LOG(LogMP, Error, TEXT("Can't Find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));

	return FMPAbilityInfo();
}
