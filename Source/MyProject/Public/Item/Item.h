// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class MYPROJECT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY()
	AActor* MPOwner = nullptr;

public:
	FVector GetItemSocketLocation(const FName& SocketName);
	void DetachFromCharacter();

	FORCEINLINE void SetMPOwner(AActor* InOwner) { MPOwner = InOwner; }
	FORCEINLINE AActor* GetMPOwner() { return MPOwner; }
};
