// EvoAnd Copyright


#include "AbilitySystem/MPAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Gameframework/Character.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MPGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/MainCharacterPlayerController.h"
#include "MPAbilitySystemBlueprintLibrary.h"
#include "MPLogChannel.h"
#include "Interaction/PlayerInterface.h"

UMPAttributeSet::UMPAttributeSet()
{
	const FMPGameplayTags& GamepalyTags = FMPGameplayTags::Get();
	
	/* Primary Attribute */
	TagsToAttributes.Add(GamepalyTags.Attribute_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Primary_Vigor, GetVigorAttribute);

	/* Secondary Attribute */
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);

	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);

	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);

	/* Resistance Attributes */
	TagsToAttributes.Add(GamepalyTags.Attribute_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GamepalyTags.Attribute_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UMPAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*Primary Attributes*/
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/* Secondary Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	/* Resistance Attributes */
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	/*Vital Atttributes*/
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMPAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UMPAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UMPAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//Source = Causer of the effect, Target = Target of the effect (owner of this AS)
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	
	if (Data.EvaluatedData.Attribute == GetInComingDamageAttribute())
	{
		const float LocalIncomingDagae = GetInComingDamage();
		SetInComingDamage(0.f);
		if (LocalIncomingDagae > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDagae;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
				SendXPEvent(Props);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FMPGameplayTags::Get().Effect_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}
			
			const bool bBlockHit = UMPAbilitySystemBlueprintLibrary::IsBlockHit(Props.EffectContextHandle);
			const bool bCriticalHit = UMPAbilitySystemBlueprintLibrary::IsCriticalHit(Props.EffectContextHandle);
			ShowFloatingText(Props, LocalIncomingDagae, bBlockHit, bCriticalHit);
		}
	}

	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalIncomingXP = GetIncomingXP();
		SetIncomingXP(0.f);
		
		//Source Character is the owner, since GA_ListenForEvent applies GE_EventBasedEffect, adding to IncomingXP
		if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
			const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
			const int32 NumLevelUps = NewLevel - CurrentLevel;
			if (NumLevelUps > 0)
			{
				const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointReward(Props.SourceCharacter, CurrentLevel);
				const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointReward(Props.SourceCharacter, CurrentLevel);

				IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
				IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
				IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

				bTopOffHealth = true;
				bTopOffMana = true;
				IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
			}

			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
		}
	}
}

void UMPAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}

	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UMPAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockHit, bool bCriticalHit)
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AMainCharacterPlayerController* PC = Cast<AMainCharacterPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockHit, bCriticalHit);
			return;
		}
		if (AMainCharacterPlayerController* PC = Cast<AMainCharacterPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockHit, bCriticalHit);
		}
	}
}

void UMPAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UMPAbilitySystemBlueprintLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

		const FMPGameplayTags& GameplayTags = FMPGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attribute_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attribute_Meta_IncomingXP, Payload);
	}
}

void UMPAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue) const
{
	//This is a helper macro that can be used in RepNotify functions to handle attributes that will be predictively modified by clients.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Strength, OldValue);
}

void UMPAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Intelligence, OldValue);
}

void UMPAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Resilience, OldValue);
}

void UMPAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Vigor, OldValue);
}

void UMPAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Armor, OldValue);
}

void UMPAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, ArmorPenetration, OldValue);
}

void UMPAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, BlockChance, OldValue);
}

void UMPAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, CriticalHitChance, OldValue);
}

void UMPAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, CriticalHitDamage, OldValue);
}

void UMPAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, CriticalHitResistance, OldValue);
}

void UMPAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, HealthRegeneration, OldValue);
}

void UMPAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, ManaRegeneration, OldValue);
}

void UMPAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Health, OldHealth);
}

void UMPAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, MaxHealth, OldMaxHealth);
}

void UMPAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, Mana, OldMana);
}

void UMPAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, MaxMana, OldMaxMana);
}

void UMPAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, FireResistance, OldFireResistance);
}

void UMPAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, LightningResistance, OldLightningResistance);
}

void UMPAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UMPAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMPAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UMPAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}