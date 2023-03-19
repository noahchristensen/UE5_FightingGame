// Fill out your copyright notice in the Description page of Project Settings.


#include "FGHUD.h"

AFGHUD::AFGHUD()
{

}

void AFGHUD::BeginPlay()
{
	Super::BeginPlay();

	if (P1ComboWidgetClass)
	{
		P1ComboWidget = CreateWidget<UComboWidget>(GetWorld(), P1ComboWidgetClass);
		if (P1ComboWidget)
		{
			P1ComboWidget->AddToViewport();
		}
	}

	if (P2ComboWidgetClass)
	{
		P2ComboWidget = CreateWidget<UComboWidget>(GetWorld(), P2ComboWidgetClass);
		if (P2ComboWidget)
		{
			P2ComboWidget->AddToViewport();
		}
	}
}

void AFGHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AFGHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AFGHUD::P1UpdateComboCount(int32 Value)
{
	if (P1ComboWidget)
	{
		P1ComboWidget->UpdateComboCount(Value);
	}
}

void AFGHUD::P1ResetCombo()
{
	if (P1ComboWidget)
	{
		P1ComboWidget->ResetCombo();
	}
}


void AFGHUD::P2UpdateComboCount(int32 Value)
{
	if (P2ComboWidget)
	{
		P2ComboWidget->UpdateComboCount(Value);
	}
}

void AFGHUD::P2ResetCombo()
{
	if (P2ComboWidget)
	{
		P2ComboWidget->ResetCombo();
	}
}
