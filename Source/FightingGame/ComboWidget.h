// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Runtime/UMG/Public/UMG.h"

#include "ComboWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API UComboWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UComboWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override; // Similar to BeginPlay

	void UpdateComboCount(int32 Value);

	void ResetCombo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) // BindWidget: Sends a Warning so that TxtCombo is implemented
		class UTextBlock* TxtCombo;

	void StoreWidgetAnimations();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

private:
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* ComboFadeAnim;
	UWidgetAnimation* ComboShakeAnim;
};
