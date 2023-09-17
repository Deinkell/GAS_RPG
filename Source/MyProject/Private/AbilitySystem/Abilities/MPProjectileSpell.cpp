// EvoAnd Copyright


#include "AbilitySystem/Abilities/MPProjectileSpell.h"
#include "Interaction/CombatInterface.h"
#include "Actor/MPProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "MPGameplayTags.h"
#include "Character/MainCharacter.h"



void UMPProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UMPProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverrdePitch, float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	if (CanSpawnProjectile())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

		if (bOverrdePitch)
		{
			Rotation.Pitch = PitchOverride;
		}


		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AMPProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMPProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// TODO
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FMPGameplayTags GameplayTags = FMPGameplayTags::Get();

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}

		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}

bool UMPProjectileSpell::CanFirebolt()
{
	CombatInterface = CombatInterface == nullptr ? Cast<ICombatInterface>(GetAvatarActorFromActorInfo()) : CombatInterface;
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor->ActorHasTag("Player"))
	{
		return CombatInterface && CombatInterface->CanCastFirebolt() && ProjectileClass;
	}
	if (AvatarActor->ActorHasTag("Enemy"))
	{
		return CombatInterface && ProjectileClass;
	}
	
	return false;
}

bool UMPProjectileSpell::CanSpawnProjectile()
{
	CombatInterface = CombatInterface == nullptr ? Cast<ICombatInterface>(GetAvatarActorFromActorInfo()) : CombatInterface;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor->ActorHasTag("Player"))
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(AvatarActor);
		return CombatInterface && MainCharacter->GetCharacterState() == ECharacterState::ECS_KatanaCombat && ProjectileClass;
	}
	if (AvatarActor->ActorHasTag("Enemy"))
	{
		return CombatInterface && ProjectileClass;
	}

	return false;
}
