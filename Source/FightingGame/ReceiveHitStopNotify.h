// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ReceiveHitStopNotify.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API UReceiveHitStopNotify : public UAnimNotify
{
	GENERATED_BODY()
	
	public:
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
