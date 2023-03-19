// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackComboNotifyState.h"

#include "FightingGameCharacter.h"

void UAttackComboNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->ComboWindowStart();
			return;
		}
	}
}

void UAttackComboNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->ComboWindowStop();
			return;
		}
	}
}
