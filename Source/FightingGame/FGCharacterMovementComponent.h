// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "FightingGameCharacter.h"

#include "FGCharacterMovementComponent.generated.h"



//class AFightingGameCharacter;

UCLASS()
class FIGHTINGGAME_API UFGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFGCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

public:
	UFUNCTION(BlueprintPure)
		bool IsDashing() const;
	UFUNCTION(BlueprintPure)
		bool IsAirDashing() const;

	void TryJump();

	void TryDash();
	void TryBackDash();
	void TryAirDash();
	void TryAirDashBack();
	void TryDashStop();

protected:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
		AFightingGameCharacter* FGCharacterOwner;

	// Jump
	virtual bool DoJump(bool bReplayingMoves) override;
	//virtual bool CanAttemptJump() override; 
	UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping / Falling")
		float JumpXVelocity = 200.f;
	unsigned int AirActions = 1;
	// Double Jump
	UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping / Falling")
		float DoubleJumpZVelocity = 400.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping / Falling")
		float DoubleJumpXVelocity = 120.f;

	// Dash
	UPROPERTY(Category = "Character Movement: Dashing", EditDefaultsOnly)
		UAnimMontage* DashMontage;
	UPROPERTY()
		UAnimInstance* AnimInstance;
	float WalkSpeed; // Set on BeginPlay
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float DashSpeed = 400.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float MinAirDashHeight = 100.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float AirDashSpeed = 800.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float AirDashTime = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float AirDashGravity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float AirDashFriction = 1.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float BackAirDashSpeed = 800.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float BackAirDashTime = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float BackAirDashGravity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dashing")
		float BackAirDashFriction = 1.f;
	float DefaultGravity = 1.f;
	float DefaultFallFriction = 0.f;
	bool bIsDashing = false;
	bool bIsAirDashing = false;
	FTimerHandle AirDashHandle;
	void AirDashStop();
};
