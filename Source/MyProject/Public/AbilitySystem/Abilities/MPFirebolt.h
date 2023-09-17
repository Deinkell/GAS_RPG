// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MPProjectileSpell.h"
#include "MPFirebolt.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPFirebolt : public UMPProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
