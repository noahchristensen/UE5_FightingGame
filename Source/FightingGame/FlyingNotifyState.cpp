// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingNotifyState.h"

#include "FightingGameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFlyingNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			return;
		}
	}
}

void UFlyingNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			return;
		}
	}
}