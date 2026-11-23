#include "UI/HeartsWidget.h"

#include "Combat/HealthComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"

void UHeartsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// A Blueprint with no layout still works: the row becomes the whole widget
	if (!HeartsBox)
	{
		HeartsBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HeartsBox"));
		WidgetTree->RootWidget = HeartsBox;
	}
}

void UHeartsWidget::BindToHealth(UHealthComponent* Health)
{
	// Only one health component at a time
	if (BoundHealth.IsValid())
	{
		BoundHealth->OnHeartsChanged.RemoveDynamic(this, &UHeartsWidget::HandleHeartsChanged);
	}

	BoundHealth = Health;
	if (Health)
	{
		Health->OnHeartsChanged.AddUniqueDynamic(this, &UHeartsWidget::HandleHeartsChanged);
		Refresh(Health->GetCurrentHearts(), Health->GetMaxHearts());
	}
}

void UHeartsWidget::HandleHeartsChanged(UHealthComponent* HealthComponent, int32 CurrentHearts, int32 Delta)
{
	Refresh(CurrentHearts, HealthComponent->GetMaxHearts());
}

void UHeartsWidget::Refresh(int32 CurrentHearts, int32 MaxHearts)
{
	// Add or remove images so there is exactly one per heart
	while (Hearts.Num() < MaxHearts)
	{
		UImage* Heart = WidgetTree->ConstructWidget<UImage>();
		Heart->SetBrushFromTexture(HeartTexture);
		Heart->SetDesiredSizeOverride(HeartSize);
		// Top-aligned so a tall parent can't stretch the heart vertically
		UHorizontalBoxSlot* HeartSlot = HeartsBox->AddChildToHorizontalBox(Heart);
		HeartSlot->SetPadding(FMargin(2.f, 0.f));
		HeartSlot->SetVerticalAlignment(VAlign_Top);
		Hearts.Add(Heart);
	}
	while (Hearts.Num() > MaxHearts)
	{
		Hearts.Pop()->RemoveFromParent();
	}

	// Full hearts keep their colours; lost ones turn grey and see-through
	for (int32 Index = 0; Index < Hearts.Num(); ++Index)
	{
		Hearts[Index]->SetColorAndOpacity(Index < CurrentHearts ? FLinearColor::White : LostTint);
	}
}
