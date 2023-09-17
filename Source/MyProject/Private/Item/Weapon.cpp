// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/BoxComponent.h"
#include "MPAbilitySystemBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

AWeapon::AWeapon()
{
	bReplicates = true;

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(RootComponent);

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(RootComponent);
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(RootComponent);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	WeaponBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnBoxEndOverlap);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IgnoreActors.AddUnique(GetMPOwner());
	for (auto& Actor : IgnoreActors)
	{
		if (Actor == OtherActor) return;
	}

	if (HasAuthority())
	{
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetMPOwner());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(this);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = GetActorLocation();
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, ICombatInterface::Execute_GetPlayerLevel(GetMPOwner()), EffectContextHandle);

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(ICombatInterface::Execute_GetPlayerLevel(GetMPOwner()));
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	IgnoreActors.AddUnique(OtherActor);
}

void AWeapon::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::SetBoxCollisionEnabled()
{ 
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::SetBoxCollisionDisabled()
{ 
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::SetIgnoreActorClear()
{
	IgnoreActors.Empty(); 
}
