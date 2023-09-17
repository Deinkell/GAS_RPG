// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MPAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class MYPROJECT_API AMPAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMPAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
