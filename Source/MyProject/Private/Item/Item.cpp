
#include "Item/Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/TransformNonVectorized.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ItemMesh);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

FVector AItem::GetItemSocketLocation(const FName& SocketName)
{
	return ItemMesh->GetSocketLocation(SocketName);
}

void AItem::DetachFromCharacter()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	ItemMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

