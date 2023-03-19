// Fill out your copyright notice in the Description page of Project Settings.


#include "FGPlayerController.h"

#include "FightingGameCharacter.h"

void AFGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Controller and Character set"))

	//PlayerTwoController = GetWorld()->SpawnActor<APlayerController>(PlayerTwoControllerClass, PlayerTwoLocation, PlayerTwoRotation);
	//PlayerTwoCharacter = GetWorld()->SpawnActor<AFightingGameCharacter>(PlayerTwoCharacterClass, PlayerTwoLocation, PlayerTwoRotation);
	//PlayerTwoController->Possess(PlayerTwoCharacter);
}