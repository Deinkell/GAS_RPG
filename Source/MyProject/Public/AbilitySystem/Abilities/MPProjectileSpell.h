// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MPDamageGameplayAbility.h"
#include "MPProjectileSpell.generated.h"

class ICombatInterface;
class AMPProjectile;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPProjectileSpell : public UMPDamageGameplayAbility
{
	GENERATED_BODY()
	
public:


protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverrdePitch = false, float PitchOverride = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMPProjectile> ProjectileClass;

	ICombatInterface* CombatInterface;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;

public:
	UFUNCTION(BlueprintCallable)
	bool CanFirebolt();

	UFUNCTION(BlueprintCallable)
	bool CanSpawnProjectile();
};
