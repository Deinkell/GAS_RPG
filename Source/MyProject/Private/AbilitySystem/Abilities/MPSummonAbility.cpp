// EvoAnd Copyright


#include "AbilitySystem/Abilities/MPSummonAbility.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UMPSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;

	//const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}

		SpawnLocations.Add(ChosenSpawnLocation);
	}

	/*NavLocation NavLoc;
	UNavigationSystemV1* MyNavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (bool bProjectedLocationValid = MyNavSystem->ProjectPointToNavigation(ChosenSpawnLocation, NavLoc))
	{
		SpawnLocations.Add(NavLoc.Location);
	}  */
	//Find Summon location by using NavSystem

	return SpawnLocations;
}

TSubclassOf<APawn> UMPSummonAbility::GetRandomMinonClass()
{
	if (MinionClasses.Num() <= 0) return nullptr;

	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	
	return MinionClasses[Selection];
}
