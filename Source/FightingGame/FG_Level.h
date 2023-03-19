// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "FG_Level.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API AFG_Level : public ALevelScriptActor
{
	GENERATED_BODY()
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class TSubclassOf<ACameraActor> CameraClass;


	public:
		AFG_Level();

		virtual void BeginPlay() override;
};
