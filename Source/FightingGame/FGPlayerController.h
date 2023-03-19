// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FGPlayerController.generated.h"


class AFightingGameCharacter;

UCLASS()
class FIGHTINGGAME_API AFGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:
		virtual void BeginPlay() override;

	public:
	/*	UPROPERTY(EditDefaultsOnly)
			TSubclassOf<APlayerController> PlayerTwoControllerClass;
		UPROPERTY(EditDefaultsOnly)
			TSubclassOf<AFightingGameCharacter> PlayerTwoCharacterClass;

	protected:
		APlayerController* PlayerTwoController;
		AFightingGameCharacter* PlayerTwoCharacter;

		UPROPERTY(EditDefaultsOnly)
			FVector PlayerTwoLocation = FVector(-130.f, -125.f, 96.f);
		UPROPERTY(EditDefaultsOnly)
			FRotator PlayerTwoRotation = FRotator(0.f, -90.f, 0.f);*/
};
