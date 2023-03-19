// Copyright Epic Games, Inc. All Rights Reserved.


#include "FightingGameGameModeBase.h"

#include "FightingGameCharacter.h"
#include "Kismet/GameplayStatics.h"

AFightingGameGameModeBase::AFightingGameGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AFightingGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnCharacters();
}

void AFightingGameGameModeBase::SpawnCharacters()
{
	PlayerOne = GetWorld()->SpawnActor<AFightingGameCharacter>(PlayerOneCharacter, PlayerOneSpawnLocation, FRotator(0.f, 0.f, 0.f));
	PlayerTwo = GetWorld()->SpawnActor<AFightingGameCharacter>(PlayerTwoCharacter, PlayerTwoSpawnLocation, FRotator(0.f, 180.f, 0.f));
	PlayerOne->SetOtherCharacter(PlayerTwo);
	PlayerTwo->SetOtherCharacter(PlayerOne);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(PlayerOne);
	UGameplayStatics::CreatePlayer(GetWorld(), -1, true)->Possess(PlayerTwo);
}
