// EvoAnd Copyright

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "MPInputConfig.h"

#include "MPInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMPInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFunctType, typename HeldFuncType>
	void BindAbilityActions(const UMPInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFunctType ReleasedFunc, HeldFuncType HeldFunc);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFunctType, typename HeldFuncType>
inline void UMPInputComponent::BindAbilityActions(const UMPInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFunctType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FMPInputAction& Action : InputConfig->AbilityInputAction)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
