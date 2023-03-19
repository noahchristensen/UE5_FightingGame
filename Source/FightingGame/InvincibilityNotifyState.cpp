// Fill out your copyright notice in the Description page of Project Settings.


#include "InvincibilityNotifyState.h"

#include "FightingGameCharacter.h"

void UInvincibilityNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->StartInvincibility();
			return;
		}
	}
}

void UInvincibilityNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->StopInvincibility();
			return;
		}
	}
}