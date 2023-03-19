// Fill out your copyright notice in the Description page of Project Settings.


#include "ReceiveHitStopNotify.h"

#include "FightingGameCharacter.h"

void UReceiveHitStopNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->ReceiveHitStop();
		}
	}
}