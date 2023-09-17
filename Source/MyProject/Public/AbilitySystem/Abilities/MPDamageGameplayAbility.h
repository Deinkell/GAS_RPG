// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MPGameplayAbility.h"
#include "Interaction/CombatInterface.h"

#include "MPDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPDamageGameplayAbility : public UMPGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	float GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType);
};
