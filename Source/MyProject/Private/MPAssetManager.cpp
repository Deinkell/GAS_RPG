// EvoAnd Copyright


#include "MPAssetManager.h"
#include "MPGameplayTags.h"
#include "AbilitySystemGlobals.h"

UMPAssetManager& UMPAssetManager::Get()
{
	check(GEngine);
	UMPAssetManager* MPAssetManager = Cast<UMPAssetManager>(GEngine->AssetManager);
	return *MPAssetManager;
}

void UMPAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FMPGameplayTags::InitializeNativeGameplayTags();

	//This is Required on Target Data
	UAbilitySystemGlobals::Get().InitGlobalData();
}
