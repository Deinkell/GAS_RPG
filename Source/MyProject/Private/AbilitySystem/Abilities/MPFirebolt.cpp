// EvoAnd Copyright


#include "AbilitySystem/Abilities/MPFirebolt.h"
#include "MPGameplayTags.h"

FString UMPFirebolt::GetDescription(int32 Level)
{
	const int Damage = GetDamageByDamageType(Level, FMPGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>FIRE BOLT</>\n\n"

			//Level
			"<Small>Level : </><Level>%d</>\n"
			
			//ManaCost
			"<Small>ManaCost : </>"
			"<ManaCost>%.1f</>\n"

			//Cooldown
			"<Small>Cooldown : </><Cooldown> %0.1f</>\n\n"

			"<Default>Launched a bolt of fire, "
			"exploding on impact and dealing : </>"
			
			//Damage
			"<Damage>%d</>"
			"<Default> fire damage with a chance to burn</>\n\n"
			
		), Level, ManaCost, Cooldown, Damage);
	}
	else
	{
		return FString::Printf(TEXT(
			"<Title>FIRE BOLT</>\n\n"

			//Level
			"<Small>Level : </><Level>%d</>\n"

			//ManaCost
			"<Small>ManaCost : </>"
			"<ManaCost>%.1f</>\n"

			//Cooldown
			"<Small>Cooldown : </><Cooldown> %0.1f</>\n\n"

			//Num of bolts
			"<Default>Launched %d bolts of fire, "
			"exploding on impact and dealing : </>"

			//Damage
			"<Damage>%d</>"
			"<Default> fire damage with a chance to burn</>\n\n"

		), Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), Damage);
	}
}

FString UMPFirebolt::GetNextLevelDescription(int32 Level)
{
	const int Damage = GetDamageByDamageType(Level, FMPGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"

		//Level
		"<Small>Level : </><Level>%d</>\n"

		//ManaCost
		"<Small>ManaCost : </>"
		"<ManaCost>%.1f</>\n"

		//Cooldown
		"<Small>Cooldown : </><Cooldown> %0.1f</>\n\n"

		"<Default>Launched a bolt of fire, "
		"exploding on impact and dealing : </>"

		//Damage
		"<Damage>%d</>"
		"<Default> fire damage with a chance to burn</>\n\n"

	), Level, ManaCost, Cooldown, Damage);
}