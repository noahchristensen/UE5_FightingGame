// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

#include "FightingGameCharacter.h"
#include "Hitbox.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//if (!RootComponent)
	//{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	//}

	//if (!CollisionComponent)
	//{
		// Use a sphere as a simple collision representation.
	HitboxComponent = CreateDefaultSubobject<UHitBox>(TEXT("Hitbox"));

	// Event called when component hits something.
	HitboxComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
	HitboxComponent->OnComponentEndOverlap.AddDynamic(this, &AProjectile::OnOverlapEnd);

	// Set the root component to be the collision component.
	RootComponent = HitboxComponent;
	//}

	//if (!ProjectileMovementComponent)
	//{
		// Use this component to drive this projectile's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(HitboxComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	//}

	//if (!ProjectileMeshComponent)
	//{
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	//}

	ProjectileDamage = 20.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile: OnHit()"));

	//APawn* OwnerPawn = Cast<APawn>(GetOwner());
	//if (OwnerPawn == nullptr && GetOwner() != nullptr)
	//{
	//	OwnerPawn = Cast<APawn>(GetOwner()->GetOwner());
	//}

	//AFightingGameCharacter* OtherFGChar = Cast<AFightingGameCharacter>(OtherActor);
	//if (OwnerPawn == nullptr || OtherFGChar == nullptr)
	//{
	//	return;
	//}

	//AController* OwnerController = OwnerPawn->GetController();
	//FPointDamageEvent DamageEvent; // nothing for now, can add for different hit effects possibly
	//float Damage = 1;

	//if (OwnerPawn)
	//{
	//	if (OwnerController == GetWorld()->GetFirstPlayerController()) // player fired the projectile
	//	{
	//		APawn* OtherPawn = Cast<APawn>(OtherActor);
	//		if (OtherPawn && OtherPawn->GetController() != GetWorld()->GetFirstPlayerController()) // projectile hit an enemy
	//		{
	//			Damage = OtherFGChar->ProcessHit(ProjectileDamage, EGuardType::GT_Mid);
	//		}
	//	}
	//	else // enemy fired the projectile
	//	{
	//		APawn* OtherPawn = Cast<APawn>(OtherActor);
	//		if (OtherPawn && OtherPawn->GetController() == GetWorld()->GetFirstPlayerController()) // projectile hit the player
	//		{
	//			Damage = OtherFGChar->ProcessHit(ProjectileDamage, EGuardType::GT_Mid);
	//		}
	//	}
	//}


	if (DisableOnHit && OtherActor != GetOwner())
	{
		if (SplashEffect != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SplashEffect, GetActorLocation(), GetActorRotation());
		}
		Destroy();
	}
}

void AProjectile::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	HitboxComponent->Activate();
}