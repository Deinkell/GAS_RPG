// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MPHUD.generated.h"

class UMPUserWidget;
class UOverlayWidgetController;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
class USpellMenuWidgetController;
class UInventoryWidgetController;
/**
 * 
 */
UCLASS()
class MYPROJECT_API AMPHUD : public AHUD
{
	GENERATED_BODY()

public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWIdgetController(const FWidgetControllerParams& WCParams);
	USpellMenuWidgetController* GetSpellMenuWIdgetController(const FWidgetControllerParams& WCParams);
	UInventoryWidgetController* GetInventoryWIdgetController(const FWidgetControllerParams& WCParams);
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY()
	TObjectPtr<UMPUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMPUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;
};
