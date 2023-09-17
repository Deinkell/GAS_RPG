// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FMPLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;
};

/**
 * 
 */
UCLASS()
class MYPROJECT_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	int32 FindLevelForXP(int32 XP) const;

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FMPLevelUpInfo> LevelUpInformation;
	
};
