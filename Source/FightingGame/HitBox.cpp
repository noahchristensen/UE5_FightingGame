// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBox.h"

#include "FightingGameCharacter.h"
#include "Projectile.h"

UHitBox::UHitBox()
{
	OnComponentBeginOverlap.AddDynamic(this, &UHitBox::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UHitBox::OnOverlapEnd);

	SetCollisionProfileName(TEXT("Hitbox"));

	SetAbsolute(false, true, true);
}

void UHitBox::BeginPlay()
{
	Super::BeginPlay();

	SetVisibility(false);
	SetHiddenInGame(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UHitBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ensure Projectiles don't hit their owners
	AActor* ActorOwner = GetOwner();
	if (Cast<AProjectile>(ActorOwner))
	{
		ActorOwner = ActorOwner->GetOwner();
	}

	if (ActorOwner && OtherActor != ActorOwner)
	{
		if (!bDamageInflicted)
		{
			InflictDamage(OtherActor);
		}
	}
}

void UHitBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void UHitBox::Activate()
{
	bAttackHit = false;
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetVisibility(true);
}

void UHitBox::Deactivate()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bDamageInflicted = false;
	SetVisibility(false);
}

void UHitBox::InflictDamage(AActor* OtherActor)
{
	bAttackHit = true;

	bDamageInflicted = true;

	AFightingGameCharacter* FGChar = Cast<AFightingGameCharacter>(OtherActor);
	if (FGChar)
	{
		FGChar->ProcessHit(Damage, GuardType, HitProperties);
		FGChar->StartHitstop(HitstunDuration, HitstunDilation);
	}

	AFightingGameCharacter* FGOwner = Cast<AFightingGameCharacter>(GetOwner());
	if (FGOwner)
	{
		FGOwner->StartHitstop(HitstopDuration, 0.02f);
	}
}
