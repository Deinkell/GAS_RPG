// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FMPAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FMPAttributeInfo FindAttributeInfo(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMPAttributeInfo> AttributeInformation;
};
