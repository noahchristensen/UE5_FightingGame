// Fill out your copyright notice in the Description page of Project Settings.


#include "FG_Level.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

AFG_Level::AFG_Level()
{
	
}

void AFG_Level::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->SpawnActor(CameraClass);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	//PlayerController->SetViewTargetWithBlend(FollowCamera)

}
