// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UHitBox;

UCLASS()
class FIGHTINGGAME_API AProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Projectile)
		class UStaticMeshComponent* ProjectileMeshComponent;
	UPROPERTY(EditAnywhere, Category = Projectile)
		class UHitBox* HitboxComponent;
	UPROPERTY(EditAnywhere, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovementComponent;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Function that initializes the projectile's velocity in the shoot direction.
	void FireInDirection(const FVector& ShootDirection);

	UPROPERTY(EditAnywhere, Category = "Stats")
		float ProjectileDamage;

	UPROPERTY(EditAnywhere, Category = "Stats")
		float LifeSpan = 3.f;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UParticleSystem* SplashEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Extra")
		bool DisableOnHit = true;

};
