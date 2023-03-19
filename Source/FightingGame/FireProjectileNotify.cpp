// Fill out your copyright notice in the Description page of Project Settings.


#include "FireProjectileNotify.h"

#include "FightingGameCharacter.h"

void UFireProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AFightingGameCharacter* OwnerChar = Cast<AFightingGameCharacter>(MeshComp->GetOwner());
		if (OwnerChar != nullptr)
		{
			OwnerChar->FireProjectile();
		}
	}
}