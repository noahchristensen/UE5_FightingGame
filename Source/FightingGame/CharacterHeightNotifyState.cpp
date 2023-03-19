// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHeightNotifyState.h"

#include "FightingGameCharacter.h"

void UCharacterHeightNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->SetCharacterHeight(NewHeight);
			return;
		}
	}
}

void UCharacterHeightNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->ResetCharacterHeight();
			return;
		}
	}
}
