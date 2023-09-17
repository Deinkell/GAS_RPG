// EvoAnd Copyright


#include "Character/EnemyCharacter.h"
#include "MyProject/MyProject.h"
#include "AbilitySystem/MPAbilitySystemComponent.h"
#include "AbilitySystem/MPAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/MPUserWIdget.h"
#include "MPAbilitySystemBlueprintLibrary.h"
#include "MPGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/MPAIController.h" 
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"


AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UMPAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UMPAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("WidgetComopnent");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	MPAIController = Cast<AMPAIController>(NewController);

	MPAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	MPAIController->RunBehaviorTree(BehaviorTree);
	MPAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	MPAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AEnemyCharacter::HightlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	DefaultWeapon->SetRenderCustomDepth(true);
	DefaultWeapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemyCharacter::UnHightlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	DefaultWeapon->SetRenderCustomDepth(false);
}

void AEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpped;
	InitAbilityActorInfo();

	if(HasAuthority())	UMPAbilitySystemBlueprintLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	
	if (UMPUserWidget* MPUserWidget = Cast<UMPUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		MPUserWidget->SetWidgetController(this);
	}

	const UMPAttributeSet* MPAS = CastChecked<UMPAttributeSet>(AttributeSet);
	if(MPAS)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MPAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) 
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MPAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
	
		AbilitySystemComponent->RegisterGameplayTagEvent(FMPGameplayTags::Get().Effect_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &ThisClass::HitReactChanged
		);

		OnHealthChanged.Broadcast(MPAS->GetHealth());
		OnMaxHealthChanged.Broadcast(MPAS->GetMaxHealth());
	}
}

int32 AEnemyCharacter::GetPlayerLevel_Implementation()
{
	return Level;
}

void AEnemyCharacter::HitReactChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpped;
	
	if (MPAIController && MPAIController->GetBlackboardComponent())
	{
		MPAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}		
}

void AEnemyCharacter::Die()
{
	SetLifeSpan(LifeSpn);

	if(MPAIController) MPAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);

	Super::Die();
}

void AEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UMPAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if(HasAuthority())	InitializeDefaultAttributes();
}

void AEnemyCharacter::InitializeDefaultAttributes()
{
	UMPAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
