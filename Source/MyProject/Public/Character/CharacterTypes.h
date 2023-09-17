#pragma once

UENUM(BlueprintType)
enum ECustomMovementMode
{
	ECMOVE_Climbing UMETA(DisplayName = "Climbing"),
	ECMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_UnEquipped UMETA(DisplayName = "Unequippd"),
	ECS_KatanaCombat UMETA(DisplayName = "KatanaCombat")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Climb UMETA(DisplayName = "Climb"),
	EAS_Walk UMETA(DisplayName = "Walk"),
	EAS_Run UMETA(DisplayName = "Run"),
	EAS_Jump UMETA(DisplayName = "Jump"),
	EAS_CancelClimb UMETA(DisplayName = "CancelClimbing"),
	EAS_LedgeUpClimb UMETA(DisplayName = "LedgeUpClimbing"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_UnequippingWeapon UMETA(DisplayName = "UnEquipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_ChangingClimbing UMETA(DisplayName = "ChangingClimbing"),
	EAS_ClimbDash UMETA(DisplayName = "ClimbDash"),
	EAS_Death UMETA(DisplayName = "Death")
};


UENUM(BlueprintType)
enum class ETurnInPlace : uint8
{
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),
	ETIP_Left UMETA(DisplayName = "Tuning Left"),
	ETIP_Right UMETA(DisplayName = "Tuning Right"),

	ETIP_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EComboState : uint8
{
	ECS_NoCombo UMETA(DisplayName = "NoCombo"),
	ECS_KatanaCombo_1 UMETA(DisplayName = "KatanaCombo_1"),

	ECS_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),

	EDP_Max UMETA(DisplayName = "EDP_Max")
};

