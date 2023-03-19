// Fill out your copyright notice in the Description page of Project Settings.


#include "FGCharacterMovementComponent.h"

#include "FightingGameCharacter.h"
#include "TimerManager.h"

UFGCharacterMovementComponent::UFGCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UFGCharacterMovementComponent::BeginPlay()
{
	FGCharacterOwner = Cast<AFightingGameCharacter>(CharacterOwner);

	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
	DefaultGravity = GravityScale;
	DefaultFallFriction = FallingLateralFriction;
}


void UFGCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == EMovementMode::MOVE_Walking)
	{
		if (IsDashing())
		{
			//DashStop();
		}
		else if (IsCrouching())
		{
			UnCrouch();
			FGCharacterOwner->OnCrouchStop();
		}
	}
	else if (PreviousMovementMode == EMovementMode::MOVE_Falling)
	{
		AirActions = 1; 
	}
}


void UFGCharacterMovementComponent::TryJump()
{
	if (FGCharacterOwner && FGCharacterOwner->GetCharacterState() == CS_Default)
	{
		DoJump(false);
	}
}

bool UFGCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		// Don't jump if we can't move up/down.
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity);
			if (FGCharacterOwner && FMath::Abs(FGCharacterOwner->GetForwardInput()) > 0.5)
			{
				// Handles Forward and Backward Jumps
				float unsignedVelocity = FMath::Max<FVector::FReal>(FMath::Abs(Velocity.X) + (JumpXVelocity/4), JumpXVelocity);
				Velocity.X = unsignedVelocity * FMath::Sign(FGCharacterOwner->GetForwardInput());
			}

			SetMovementMode(MOVE_Falling);
			return true;
		}
	}
	else if (FGCharacterOwner && FGCharacterOwner->CanAirJump() && AirActions > 0)
	{
		// Don't jump if we can't move up/down.
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			AirActions--;

			Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, DoubleJumpZVelocity);
			if (FMath::Abs(FGCharacterOwner->GetForwardInput()) > 0.5)
			{
				if (FMath::Sign(Velocity.X) == FMath::Sign(FGCharacterOwner->GetForwardInput()))
				{
					float unsignedVelocity = FMath::Max<FVector::FReal>(Velocity.X, DoubleJumpXVelocity);
					Velocity.X = unsignedVelocity * FMath::Sign(FGCharacterOwner->GetForwardInput());
				}
				else
				{
					Velocity.X = DoubleJumpXVelocity * FMath::Sign(FGCharacterOwner->GetForwardInput());
				}
			}

			// Movementmode already MOVE_Falling
			return true;
		}
	}

	return false;
}




bool UFGCharacterMovementComponent::IsDashing() const
{
	return bIsDashing;
}


bool UFGCharacterMovementComponent::IsAirDashing() const
{
	return bIsAirDashing;
}

void UFGCharacterMovementComponent::TryDash()
{
	MaxWalkSpeed = DashSpeed;
	Velocity = CharacterOwner->GetActorForwardVector() * DashSpeed;
	bIsDashing = true;
}

void UFGCharacterMovementComponent::TryDashStop()
{
	MaxWalkSpeed = WalkSpeed;
	bIsDashing = false;
}


void UFGCharacterMovementComponent::TryBackDash()
{
	if (AnimInstance && DashMontage && AnimInstance->Montage_IsPlaying(DashMontage))
	{
		return;
	}

	//SetMovementMode(EMovementMode::MOVE_Flying);
	if (FGCharacterOwner)
	{
		FGCharacterOwner->SetCharacterState(CS_Dashing);
	}

	AnimInstance->Montage_Play(DashMontage);
}

void UFGCharacterMovementComponent::TryAirDash()
{
	if (FGCharacterOwner == nullptr || AirActions == 0 || FGCharacterOwner->GetActorLocation().Z < MinAirDashHeight)
	{
		return;
	}
	bIsAirDashing = true;
	AirActions--;
	if (FGCharacterOwner)
	{
		FGCharacterOwner->SetCharacterState(CS_Dashing);
	}

	Velocity.Z = 0.f;
	GravityScale = AirDashGravity;
	FallingLateralFriction = AirDashFriction;


	//Velocity.X = AirDashSpeed * FGCharacterOwner->GetForwardInput();
	Launch(AirDashSpeed * CharacterOwner->GetActorForwardVector());
	HandlePendingLaunch();

	GetWorld()->GetTimerManager().SetTimer(AirDashHandle, this, &UFGCharacterMovementComponent::AirDashStop, AirDashTime, false);
}

void UFGCharacterMovementComponent::TryAirDashBack()
{
	if (FGCharacterOwner == nullptr || AirActions == 0 || FGCharacterOwner->GetActorLocation().Z < MinAirDashHeight)
	{
		return;
	}
	bIsAirDashing = true;
	AirActions--;
	if (FGCharacterOwner)
	{
		FGCharacterOwner->SetCharacterState(CS_Dashing);
	}

	Velocity.Z = 0.f;
	GravityScale = BackAirDashGravity;
	FallingLateralFriction = BackAirDashFriction;

	Launch(-BackAirDashSpeed * CharacterOwner->GetActorForwardVector());
	HandlePendingLaunch();
	//Velocity.X = AirDashSpeed * FGCharacterOwner->GetForwardInput();
	GetWorld()->GetTimerManager().SetTimer(AirDashHandle, this, &UFGCharacterMovementComponent::AirDashStop, BackAirDashTime, false);
}

void UFGCharacterMovementComponent::AirDashStop()
{
	if (FGCharacterOwner)
	{
		FGCharacterOwner->SetCharacterState(CS_Default);
	}
	bIsAirDashing = false;
	GravityScale = DefaultGravity;
	FallingLateralFriction = DefaultFallFriction;
}