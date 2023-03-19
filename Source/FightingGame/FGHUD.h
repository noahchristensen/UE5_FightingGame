// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Components/WidgetComponent.h"

#include "ComboWidget.h"

#include "FGHUD.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API AFGHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AFGHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void P1UpdateComboCount(int32 Value);
	UFUNCTION()
		void P1ResetCombo();
	UFUNCTION()
		void P2UpdateComboCount(int32 Value);
	UFUNCTION()
		void P2ResetCombo();

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> P1ComboWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> P2ComboWidgetClass;

private:
	UComboWidget* P1ComboWidget;
	UComboWidget* P2ComboWidget;
};
