// EvoAnd Copyright


#include "UI/Widget/MPUserWidget.h"

void UMPUserWidget::SetWidgetController(UObject* InWidgeController)
{
	WidgetController = InWidgeController;
	WidgetControllerSet();
}
