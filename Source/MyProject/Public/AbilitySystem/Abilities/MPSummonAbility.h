// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MPGameplayAbility.h"
#include "MPSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPSummonAbility : public UMPGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();
	
	UFUNCTION(BlueprintPure, Category = "Summoning")
	TSubclassOf<APawn> GetRandomMinonClass();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	float MinSpawnDistance = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoing")
	float SpawnSpread = 90.f;
};
 