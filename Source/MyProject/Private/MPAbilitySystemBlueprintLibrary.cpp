// EvoAnd Copyright


#include "MPAbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/MPHUD.h"
#include "Game/MPPlayerState.h"
#include "Game/MPGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"
#include "MPAbilityTypes.h"
#include "UI/WidgetController/MPWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystemBlueprintLibrary.h"

bool UMPAbilitySystemBlueprintLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AMPHUD*& OutMPHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMPHUD* MPHUD = Cast<AMPHUD>(PC->GetHUD()))
		{
			AMPPlayerState* PS = PC->GetPlayerState<AMPPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			OutMPHUD = MPHUD;

			return true;
		}
	}

	return false;
}

UOverlayWidgetController* UMPAbilitySystemBlueprintLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WIdgetControllerParams;
	AMPHUD* MPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WIdgetControllerParams, MPHUD))
	{
		return MPHUD->GetOverlayWidgetController(WIdgetControllerParams);
	}

	return nullptr;
}

UAttributeMenuWidgetController* UMPAbilitySystemBlueprintLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WIdgetControllerParams;
	AMPHUD* MPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WIdgetControllerParams, MPHUD))
	{
		return MPHUD->GetAttributeMenuWIdgetController(WIdgetControllerParams);
	}

	return nullptr;
}

USpellMenuWidgetController* UMPAbilitySystemBlueprintLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WIdgetControllerParams;
	AMPHUD* MPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WIdgetControllerParams, MPHUD))
	{
		return MPHUD->GetSpellMenuWIdgetController(WIdgetControllerParams);
	}

	return nullptr;
}

UInventoryWidgetController* UMPAbilitySystemBlueprintLibrary::GetInventoryWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WIdgetControllerParams;
	AMPHUD* MPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WIdgetControllerParams, MPHUD))
	{
		return MPHUD->GetInventoryWIdgetController(WIdgetControllerParams);
	}

	return nullptr;
}

void UMPAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UMPAbilitySystemBlueprintLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	
	if (CharacterClassInfo == nullptr) return;

	for (auto& AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.StartUpAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UMPAbilitySystemBlueprintLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr) return 0;

	FCharacterClassDefaultInfo Info =  CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

void UMPAbilitySystemBlueprintLibrary::ApplyEffectToMainCharacter(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 ActorLevel)
{
	if (TargetActor->ActorHasTag(FName("Enemy"))) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(TargetActor);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

UCharacterClassInfo* UMPAbilitySystemBlueprintLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AMPGameModeBase* GameMode = Cast<AMPGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;

	return GameMode->CharacterClassInfo;;
}

UAbilityInfo* UMPAbilitySystemBlueprintLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AMPGameModeBase* GameMode = Cast<AMPGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;

	return GameMode->AbilityInfo;
}

bool UMPAbilitySystemBlueprintLibrary::IsBlockHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMPGameplayEffectContext* MPEffectContext = static_cast<const FMPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MPEffectContext->IsBlockedHit();
	}

	return false;
}

bool UMPAbilitySystemBlueprintLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMPGameplayEffectContext* MPEffectContext = static_cast<const FMPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MPEffectContext->IsCriticalHit();
	}

	return false;
}

void UMPAbilitySystemBlueprintLibrary::GetLifePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, 
																														const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (auto& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UMPAbilitySystemBlueprintLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies= FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFreinds = bBothArePlayers || bBothAreEnemies;

	return !bFreinds;
}

void UMPAbilitySystemBlueprintLibrary::SetIsBlockHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockHit)
{
	if (FMPGameplayEffectContext* MPEffectContext = static_cast<FMPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MPEffectContext->SetIsBlockHit(bInIsBlockHit);
	}
}

void UMPAbilitySystemBlueprintLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FMPGameplayEffectContext* MPEffectContext = static_cast<FMPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MPEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

