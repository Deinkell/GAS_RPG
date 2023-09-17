// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MPAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UMPAssetManager& Get();

protected:
	virtual void StartInitialLoading();
};
