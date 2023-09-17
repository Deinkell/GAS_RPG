// EvoAnd Copyright


#include "Character/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "Item/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "MyProject/MyProject.h"
#include "Net/UnrealNetwork.h"
#include "MPGameplayTags.h"
#include "Component/InventoryComponent.h"
#include "Actor/InveontoryMPEffectActor.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/InventoryInterface.h"


ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	if (bUseDefaultWeapon)
	{
		DefaultWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("DefaultWeapon");
		DefaultWeapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
		DefaultWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* ABaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

FVector ABaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (ActorHasTag("Player"))
	{
		check(Weapon);
		return Weapon->GetItemSocketLocation(WeaponTipSocketName);
	}
	else if (ActorHasTag("Enemy"))
	{
		const FMPGameplayTags& GameplayTags = FMPGameplayTags::Get();
		if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && DefaultWeapon)
		{
			return DefaultWeapon->GetSocketLocation(WeaponTipSocketName);
		}

		if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
		{
			return GetMesh()->GetSocketLocation(RightHandSocketName);
		}
	
		if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
		{
			return GetMesh()->GetSocketLocation(LeftHandSocketName);
		}	

		if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
		{
			return GetMesh()->GetSocketLocation(TailSocketName);
		}
	}

	return FVector();
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation() 
{
	return this;
}

TArray<FTaggedMontage> ABaseCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* ABaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ABaseCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag) return TaggedMontage;
	}

	return FTaggedMontage();
}

int32 ABaseCharacter::GetMinionCount_Implementation()
{
	return MinionCount;
}

void ABaseCharacter::IncreamentMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass ABaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FVector ABaseCharacter::GetCharacterLocation()
{
	return GetActorLocation();
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, Weapon);
}

void ABaseCharacter::AddInventoryItem(FMPInventoryItem& InItem, int32 InventoryNumber)
{
	InItem.InventoryNum = InventoryNumber;
	if(Inventory && Inventory->Implements<UInventoryInterface>())
		IInventoryInterface::Execute_SetInventoryItem(Inventory, InItem);	
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::InitAbilityActorInfo()
{
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeDefaultAttributes()
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ABaseCharacter::AddCharacterAbilities()
{
	if (!HasAuthority()) return;

	UMPAbilitySystemComponent* MPASC = CastChecked<UMPAbilitySystemComponent>(AbilitySystemComponent);
	MPASC->AddCharacterAbilities(StartupAbilities);
	MPASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void ABaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}

	if (IsValid(WeaopnDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaopnDissolveMaterialInstance, this);
		DefaultWeapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

void ABaseCharacter::Die()
{
	DefaultWeapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

void ABaseCharacter::MulticastHandleDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

	DefaultWeapon->SetSimulatePhysics(true);
	DefaultWeapon->SetEnableGravity(true);
	DefaultWeapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
}

