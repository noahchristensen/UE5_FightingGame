// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStopNotify.h"

#include "FightingGameCharacter.h"

void UAttackStopNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->StopAttack();
		}
	}
}
