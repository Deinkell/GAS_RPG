// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgeController);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
};
