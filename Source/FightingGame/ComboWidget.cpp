// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboWidget.h"

UComboWidget::UComboWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UComboWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StoreWidgetAnimations();

	ComboFadeAnim = GetAnimationByName(TEXT("ComboFade"));
	ComboShakeAnim = GetAnimationByName(TEXT("ComboShake"));
}

void UComboWidget::UpdateComboCount(int32 Value)
{
	if (TxtCombo && Value > 1) // Display Combo Condition
	{	
		if (TxtCombo->Visibility == ESlateVisibility::Hidden)
		{
			TxtCombo->SetVisibility(ESlateVisibility::Visible);
		}
		TxtCombo->SetText(FText::FromString(FString::FromInt(Value) + "x Combo"));

		if (ComboFadeAnim)
		{
			PlayAnimation(ComboFadeAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		}

		if (ComboShakeAnim)
		{
			PlayAnimation(ComboShakeAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
	}
}

void UComboWidget::ResetCombo()
{
	if (TxtCombo)
	{
		TxtCombo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UComboWidget::StoreWidgetAnimations()
{
	AnimationsMap.Empty();

	FProperty* Prop = GetClass()->PropertyLink;

	while (Prop)
	{
		if (Prop->GetClass() == FObjectProperty::StaticClass())
		{
			FObjectProperty* ObjProp = Cast<FObjectProperty>(Prop);

			//if (ObjProp->GetClass() == UWidgetAnimation::StaticClass())
			//{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnimation && WidgetAnimation->MovieScene)
				{
					FName AnimName = WidgetAnimation->MovieScene->GetFName();
					AnimationsMap.Add(AnimName, WidgetAnimation);
				}
			//}
		}

		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UComboWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);
	if (WidgetAnimation)
	{
		return *WidgetAnimation;
	}

	return nullptr;
}
