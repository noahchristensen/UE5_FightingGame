// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FightingGameGameModeBase.generated.h"


class AFightingGameCharacter;

UCLASS()
class FIGHTINGGAME_API AFightingGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* MainCamera;

	public:
		AFightingGameGameModeBase();

	public:
		virtual void BeginPlay() override;

		virtual void SpawnCharacters();

		UFUNCTION(BlueprintPure)
		inline AFightingGameCharacter* GetPlayerOne() { return PlayerOne; }
		UFUNCTION(BlueprintPure)
		inline AFightingGameCharacter* GetPlayerTwo() { return PlayerTwo; }
		
		

	protected:
		UPROPERTY(EditAnywhere)
			TSubclassOf<AFightingGameCharacter> PlayerOneCharacter;
		UPROPERTY(EditAnywhere)
			TSubclassOf<AFightingGameCharacter> PlayerTwoCharacter;
		UPROPERTY(EditDefaultsOnly)
			FVector PlayerOneSpawnLocation = FVector(130.f, -125.f, 96.f);
		UPROPERTY(EditDefaultsOnly)
			FVector PlayerTwoSpawnLocation = FVector(-130.f, -125.f, 96.f);

		AFightingGameCharacter* PlayerOne;
		AFightingGameCharacter* PlayerTwo;
};
