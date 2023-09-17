// EvoAnd Copyright


#include "Interaction/CombatInterface.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.

FVector ICombatInterface::GetCharacterLocation()
{
	return FVector();
}

bool ICombatInterface::CanCastFirebolt()
{
	return false;
}
