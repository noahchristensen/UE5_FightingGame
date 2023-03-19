// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "HitBox.generated.h"

UENUM(BlueprintType)
enum EGuardType
{
	GT_NoGuard = 0	UMETA(DisplayName = "NONE"),
	GT_Mid = 1		UMETA(DisplayName = "MID"),
	GT_Low = 2		UMETA(DisplayName = "LOW"),
	GT_High = 3		UMETA(DisplayName = "HIGH"),
	GT_Cross = 4	UMETA(DisplayName = "CROSS")
};

USTRUCT(BlueprintType)
struct FHitProperties
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		bool bSweep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FVector LaunchVector;
};


UCLASS()
class FIGHTINGGAME_API UHitBox : public UBoxComponent
{
	GENERATED_BODY()
	
	public:
		UHitBox();

	protected:
		virtual void BeginPlay() override;

	public:
		UFUNCTION()
			void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		UFUNCTION()
			void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		void Activate();
		void Deactivate();

		inline bool GetAttackHit() { return bAttackHit; }

	protected:
		void InflictDamage(AActor* OtherActor);

		UPROPERTY(EditAnywhere)
			TEnumAsByte<EGuardType> GuardType = GT_Mid;

		UPROPERTY(EditAnywhere)
			FHitProperties HitProperties;

		UPROPERTY(EditAnywhere)
			float Damage = 30.f;
		UPROPERTY(EditAnywhere)
			float HitstopDuration = 0.1f;
		UPROPERTY(EditAnywhere)
			float HitstunDuration = 0.2f;
		UPROPERTY(EditAnywhere)
			float HitstunDilation = 0.5f;
		
		bool bDamageInflicted = false;

		bool bAttackHit; // for use in comboing
};
