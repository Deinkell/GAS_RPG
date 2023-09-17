// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"


#include "MPAbilitySystemBlueprintLibrary.generated.h"

class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
struct FWidgetController;
class UAbilityInfo;
class UInventoryWidgetController;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "UMPAbilitySystemBlueprintLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AMPHUD*& OutMPHUD);

	UFUNCTION(BlueprintPure, Category = "UMPAbilitySystemBlueprintLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UMPAbilitySystemBlueprintLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UMPAbilitySystemBlueprintLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UMPAbilitySystemBlueprintLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UInventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary|CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(Blueprintpure, Category = "UMPAbilitySystemBlueprintLibrary | GameplayEffectS")
	static bool IsBlockHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(Blueprintpure, Category = "UMPAbilitySystemBlueprintLibrary | GameplayEffectS")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary | GameplayEffectS")
	static void SetIsBlockHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockHit);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary | GameplayEffectS")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "UMPAbilitySystemBlueprintLibrary | GameplayMechanics")
	static void GetLifePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintPure, Category = "UMPAbilitySystemBlueprintLibrary | GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);

	UFUNCTION(BlueprintCallable)
	static void ApplyEffectToMainCharacter(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 ActorLevel);
};
