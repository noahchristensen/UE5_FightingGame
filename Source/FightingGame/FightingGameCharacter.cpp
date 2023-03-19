// Fill out your copyright notice in the Description page of Project Settings.


#include "FightingGameCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

#include "FGCharacterMovementComponent.h"
#include "FGHUD.h"
#include "FightingGameGameModeBase.h"
#include "Projectile.h"

AFightingGameCharacter::AFightingGameCharacter(const class FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UFGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	MovementComponent = Cast<UFGCharacterMovementComponent>(GetCharacterMovement());
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them

	// Fighting Game Defaults
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 220.f;
	GetCharacterMovement()->JumpZVelocity = 630.f;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->ConstrainLocationToPlane(FVector(0, 1, 0));
	//GetCapsuleComponent()->GetWalkableSlopeOverride().SetWalkableSlopeBehavior(EWalkableSlopeBehavior::WalkableSlope_Unwalkable);
	JumpMaxCount = 2;

	// Set up Hurtbox
	Hbox = CreateDefaultSubobject<UBoxComponent>("Hbox");
	Hbox->SetupAttachment(GetMesh());
	Hbox->SetCollisionProfileName(TEXT("Hurtbox"));
	Hbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Value should change runtime

	// Set up HitBoxes
	SPunchHitbox = CreateDefaultSubobject<UHitBox>("SPunchHitbox");
	SPunchHitbox->SetupAttachment(GetMesh());
	CPunchHitbox = CreateDefaultSubobject<UHitBox>("CPunchHitbox");
	CPunchHitbox->SetupAttachment(GetMesh());
	SKickHitbox = CreateDefaultSubobject<UHitBox>("SKickHitbox");
	SKickHitbox->SetupAttachment(GetMesh());
	CKickHitbox = CreateDefaultSubobject<UHitBox>("CKickHitbox");
	CKickHitbox->SetupAttachment(GetMesh());
	SweepHitbox = CreateDefaultSubobject<UHitBox>("SweepHitbox");
	SweepHitbox->SetupAttachment(GetMesh());
	Special1Hitbox = CreateDefaultSubobject<UHitBox>("Special1Hitbox");
	Special1Hitbox->SetupAttachment(GetMesh());

	// Pushing Characters
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AFightingGameCharacter::OnCompHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFightingGameCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFightingGameCharacter::OnOverlapEnd);

	CurrentGuard = GT_NoGuard;
	CurrentState = CS_Default;
}

void AFightingGameCharacter::BeginPlay()
{
	Super::BeginPlay();


	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultGravity = GetCharacterMovement()->GravityScale;
	Health = HealthMax;
	//Hurtbox->SetBoxExtent(StandingHurtboxExtent, true);

	Hbox->SetVisibility(true);
	Hbox->SetHiddenInGame(false);
	Hbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	StartPosition = GetActorLocation();
}

void AFightingGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldFlip())
	{
		FlipCharacter();
	}

	OverlapPush();
}

void AFightingGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFightingGameCharacter::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFightingGameCharacter::JumpStop);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFightingGameCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFightingGameCharacter::CrouchStop);

	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFightingGameCharacter::MoveForward);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AFightingGameCharacter::TryDash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &AFightingGameCharacter::TryDashStop);

	// Attack
	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &AFightingGameCharacter::Punch);
	PlayerInputComponent->BindAction("Kick", IE_Pressed, this, &AFightingGameCharacter::Kick);
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AFightingGameCharacter::Grab);

	// Extra
	PlayerInputComponent->BindAction("Restart Characters", IE_Pressed, this, &AFightingGameCharacter::RestartCharacters);
}

void AFightingGameCharacter::MoveForward(float Value)
{
	ProcessForwardInput(Value);

	ForwardInput = Value;
	SetGuard(Value);

	if ((Controller != nullptr) && (Value != 0.0f) && !GetMovementComponent()->IsCrouching() && CurrentState == CS_Default)
	{
		FVector Direction = FVector(1, 0, 0);

		// Need to Immediately set to walking velocity, analog and digital should be the same
		AddMovementInput(Direction, Value);
	}

	PushOtherCharacter();
}


bool AFightingGameCharacter::IsCharacterFacingLeft()
{
	// Dot product may be unneccessary
	if (FVector::DotProduct(this->GetActorForwardVector().GetSafeNormal(), FVector::ForwardVector) > 0.f)
	{
		return true;
	}

	return false;
}

bool AFightingGameCharacter::IsOtherCharacterLeft()
{
	// X is our left-right axis, use X to easily tell character positions respective to each other
	if (this->GetActorLocation().X < OtherCharacter->GetActorLocation().X)
	{
		return true;
	}

	return false;
}



float AFightingGameCharacter::ProcessHit(float Damage, EGuardType GuardType, FHitProperties Properties)
{
	if (MovementComponent->IsDashing())
	{
		MovementComponent->TryDashStop();
	}


	if (CurrentState != CS_Stunned && WasBlockSuccessful(GuardType))
	{
		Block(GuardType);
	}
	else if (!bHitInvulnerable)
	{
		ReceiveHit(Damage, Properties);
		//return TakeDamage(Damage, FDamageEvent(), nullptr, nullptr);
	}

	return 0;
}

float AFightingGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Max(0.f, Health - Damage);

	return 0.0f;
}

void AFightingGameCharacter::ReceiveHit(float DamageAmount, FHitProperties Properties)
{
	bIsReceivingHit = true;

	ComboNum++;
	UpdateComboWidget();
	GetWorldTimerManager().SetTimer(ComboHandle, this, &AFightingGameCharacter::ResetComboWidget, 1.f, false);


	UE_LOG(LogTemp, Warning, TEXT("Combo: %i"), ComboNum)

		StopAttack();
	CurrentState = CS_Stunned;

	if (Properties.bSweep)
	{
		PlayAnimMontage(HitMontage, 1, "SweepHit");
	}
	else if (Properties.LaunchVector != FVector::ZeroVector)
	{
		StopAnimMontage(GetCurrentMontage()); // Won't launch properly if playing montage
		StopAnimMontage(HitMontage);

		if (IsOtherCharacterLeft())
		{
			Properties.LaunchVector.X *= -1;
		}
		LaunchCharacter(Properties.LaunchVector, true, true);
		// ApplyDamageMomentum - Existing Function

		CurrentState = CS_Stunned;
	}
	else if (GetMovementComponent()->IsCrouching())
	{
		PlayAnimMontage(HitMontage, 1, "CrouchingHit");
	}
	else
	{
		PlayAnimMontage(HitMontage, 1, "StandingHit");
	}


	Health = FMath::Max(0.f, Health - DamageAmount);
}

void AFightingGameCharacter::Block(TEnumAsByte<EGuardType> GuardType)
{
	if(CurrentAttack != AT_NONE) 
		StopAttack();

	CurrentState = CS_Blocking;

	FVector BlockPushback = -GetActorForwardVector() * 230.f;
	//LaunchCharacter(Properties.LaunchVector, true, true);
	//GetCapsuleComponent()->AddImpulse(BlockPushback);
	MovementComponent->Velocity = BlockPushback;

	if (bCrouchHeld)
	{
		PlayAnimMontage(BlockMontage, 1.f, "CrouchBlock");
	}
	else
	{
		PlayAnimMontage(BlockMontage, 1.f, "StandBlock");
	}
}

void AFightingGameCharacter::BlockStop()
{
	CurrentState = CS_Default;

	if (bCrouchHeld && !GetMovementComponent()->IsCrouching() && !GetMovementComponent()->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAttack: Crouch"));
		OnCrouch();
	}
	else if (!bCrouchHeld && GetMovementComponent()->IsCrouching())
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAttack: CrouchStop"));
		OnCrouchStop();
	}
	else if (bJumpHeld && !GetMovementComponent()->IsFalling())
	{
		MovementComponent->TryJump();
	}
}

void AFightingGameCharacter::SetGuard(float value)
{
	// Value > 0 means holding left, Value < 0 means holding right
	// Block is successful when Holding the OPPOSITE direction of the other character
		// Value > 0 when Character is to the left

	FVector CharacterDirection = (OtherCharacter->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();

	if ((value > 0.3 && FVector::DotProduct(CharacterDirection, -FVector::ForwardVector) > 0.3f) ||
		(value < -0.3 && FVector::DotProduct(CharacterDirection, FVector::ForwardVector) > 0.3f))
	{
		if (bCrouchHeld)
		{
			CurrentGuard = GT_Low;
		}
		else
		{
			CurrentGuard = GT_High;
		}
	}
	else
	{
		CurrentGuard = GT_NoGuard;
	}
}

bool AFightingGameCharacter::WasBlockSuccessful(EGuardType GuardType)
{
	switch (CurrentGuard)
	{
		case GT_High:
			if (GuardType == GT_Mid || GuardType == GT_High)
				return true;
			break;
		case GT_Low:
			if (GuardType == GT_Mid || GuardType == GT_Low)
				return true;
			break;
	}

	return false;
}



void AFightingGameCharacter::ReceiveHitStop()
{
	if (GetCharacterMovement()->IsWalking())
	{
		CurrentState = CS_Default;
		bIsReceivingHit = false;
		ComboNum = 0;
	}

	if (bCrouchHeld && !GetMovementComponent()->IsCrouching() && !GetMovementComponent()->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAttack: Crouch"));
		OnCrouch();
	}
	else if (!bCrouchHeld && GetMovementComponent()->IsCrouching())
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAttack: Crouch"));
		OnCrouchStop();
	}
	else if (bJumpHeld && !GetMovementComponent()->IsFalling())
	{
		MovementComponent->TryJump();
	}
}

void AFightingGameCharacter::StartHitstop(float HitstopDuration, float Dilation)
{
	CustomTimeDilation = Dilation;
	GetWorldTimerManager().SetTimer(HitstopHandle, this, &AFightingGameCharacter::StopHitstop, HitstopDuration, false);
}

void AFightingGameCharacter::StopHitstop()
{
	CustomTimeDilation = 1.f;
}


void AFightingGameCharacter::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void AFightingGameCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == OtherCharacter)
	{
		bIsOverlapping = true;
	}
}

void AFightingGameCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OtherCharacter)
	{
		bIsOverlapping = false;
	}
}


void AFightingGameCharacter::FlipCharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("Flip"));
	SetActorRotation(GetActorRotation() + FRotator(0.f, 180.f, 0.f));
}

bool AFightingGameCharacter::ShouldFlip()
{
	if (OtherCharacter == nullptr || CurrentState != CS_Default || GetMovementComponent()->IsFalling())
	{
		return false; 
	}

	FVector CharacterDirection = (OtherCharacter->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
	if (FVector::DotProduct(GetActorForwardVector(), CharacterDirection) < 0) // Direction to OtherChar is not the same as forwardvector
	{
		return true;
	}

	return false;
}

void AFightingGameCharacter::PushOtherCharacter()
{
	if (FMath::Abs(this->GetActorLocation().X - OtherCharacter->GetActorLocation().X) < MinDistancePushX &&
		FMath::Abs(this->GetActorLocation().Z - OtherCharacter->GetActorLocation().Z) < MinDistancePushZ)
	{
		//
		FVector Offset = -OtherCharacter->GetActorForwardVector() * GetWorld()->GetDeltaSeconds() * FMath::Abs(GetVelocity().X);
		OtherCharacter->AddActorWorldOffset(Offset, true, nullptr, ETeleportType::None);
	}
}

void AFightingGameCharacter::OverlapPush()
{
	if (bIsOverlapping)
	{
		FVector Offset = -OtherCharacter->GetActorForwardVector() * GetWorld()->GetDeltaSeconds() * 10.f;
		OtherCharacter->AddActorWorldOffset(Offset, true, nullptr, ETeleportType::None);
	}
}


void AFightingGameCharacter::RestartCharacters()
{
	UE_LOG(LogTemp, Warning, TEXT("Reset"));

	this->ResetCharacter();
	OtherCharacter->ResetCharacter();
}

void AFightingGameCharacter::ResetCharacter()
{
	Health = HealthMax;
	SetActorLocation(StartPosition);
	CurrentState = CS_Default;
	StopAnimMontage(GetCurrentMontage());
}



void AFightingGameCharacter::ActivateHitbox(AttackType HitboxID)
{
	switch (HitboxID)
	{
		case AT_StandingPunch:
			SPunchHitbox->Activate();
			break;
		case AT_CrouchingPunch:
			CPunchHitbox->Activate();
			break;
		case AT_StandingKick:
			SKickHitbox->Activate();
			break;
		case AT_CrouchingKick:
			CKickHitbox->Activate();
			break;
		case AT_Sweep:
			SweepHitbox->Activate();
			break;
		case AT_Special1:
			Special1Hitbox->Activate();
			break;
	}
}

void AFightingGameCharacter::DeactivateHitbox(AttackType HitboxID)
{
	switch (HitboxID)
	{
		case AT_StandingPunch:
			SPunchHitbox->Deactivate();
			break;
		case AT_CrouchingPunch:
			CPunchHitbox->Deactivate();
			break;
		case AT_StandingKick:
			SKickHitbox->Deactivate();
			break;
		case AT_CrouchingKick:
			CKickHitbox->Deactivate();
			break;
		case AT_Sweep:
			SweepHitbox->Deactivate();
			break;
		case AT_Special1:
			Special1Hitbox->Deactivate();
			break;
	}
}

void AFightingGameCharacter::StopAttack()
{
	CurrentState = CS_Default;
	CurrentAttack = AT_NONE;
	bInComboWindow = false;
	BufferedAttack = AT_NONE;

	if (bCrouchHeld && !GetMovementComponent()->IsCrouching() && !GetMovementComponent()->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAttack: Crouch"));
		OnCrouch();
	}
	else if (!bCrouchHeld && GetMovementComponent()->IsCrouching())
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAttack: CrouchStop"));
		OnCrouchStop();
	}
	else if (bJumpHeld && !GetMovementComponent()->IsFalling())
	{
		MovementComponent->TryJump();
	}
}

void AFightingGameCharacter::ComboWindowStart()
{
	bInComboWindow = true;
	if (BufferedAttack != AT_NONE)
	{
		TryAttack(BufferedAttack);
	}
}

void AFightingGameCharacter::ComboWindowStop()
{
	bInComboWindow = false;
}

void AFightingGameCharacter::Grab()
{
	if (bCrouchHeld)
	{
		ProcessButton("Sweep");


		TryAttack(AT_Sweep);
	}
}


void AFightingGameCharacter::Punch()
{
	ProcessButton("Punch");

	if (bCrouchHeld)
	{
		TryAttack(AT_CrouchingPunch);
	}
	else if (GetMovementComponent()->IsFalling())
	{
		TryAttack(AT_JumpingPunch);
	}
	else
	{
		TryAttack(AT_StandingPunch);
	}
}

void AFightingGameCharacter::Kick()
{
	ProcessButton("Kick");

	if (bCrouchHeld)
	{
		TryAttack(AT_CrouchingKick);
	}
	else if (GetMovementComponent()->IsFalling())
	{
		TryAttack(AT_JumpingKick);
	}
	else
	{
		TryAttack(AT_StandingKick);
	}
}

void AFightingGameCharacter::TryAttack(AttackType Attack)
{
	if (bInComboWindow && (MoveWithinGatlings(Attack) && CheckIfHit()) ||
		(CurrentState == CS_Default))
	{
		CurrentState = CS_Attacking;
		BufferedAttack = AT_NONE;
		CurrentAttack = Attack;
		FText AttackName;
		UEnum::GetDisplayValueAsText(Attack, AttackName);
		PlayAnimMontage(AttackMontage, 1, FName(AttackName.ToString()));
	}
	else
	{
		BufferedAttack = Attack;
	}
}

bool AFightingGameCharacter::MoveWithinGatlings(AttackType CheckMove)
{
	switch (CurrentAttack)
	{
	case AT_StandingPunch:
		return SPunchGatlings.Find(CheckMove) != INDEX_NONE;
		break;
	case AT_CrouchingPunch:
		return CPunchGatlings.Find(CheckMove) != INDEX_NONE;
		break;
	case AT_StandingKick:
		return SKickGatlings.Find(CheckMove) != INDEX_NONE;
		break;
	case AT_CrouchingKick:
		return CKickGatlings.Find(CheckMove) != INDEX_NONE;
		break;
	}

	return false;
}

bool AFightingGameCharacter::SpecialCancelable(FCommand Command)
{
	return true;
}

const bool AFightingGameCharacter::CheckIfHit()
{
	switch (CurrentAttack)
	{
	case AT_StandingPunch:
		return SPunchHitbox->GetAttackHit();
		break;
	case AT_CrouchingPunch:
		return CPunchHitbox->GetAttackHit();
		break;
	case AT_StandingKick:
		return SKickHitbox->GetAttackHit();
		break;
	case AT_CrouchingKick:
		return CKickHitbox->GetAttackHit();
		break;
	}

	return false;
}

void AFightingGameCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == EMovementMode::MOVE_Falling)
	{
		if (CurrentState == CS_Attacking)
		{
			StopAnimMontage(AttackMontage);
			StopAttack();
		}

		if (bCrouchHeld && GetCharacterMovement()->IsWalking())
		{
			OnCrouch();
		}
		if (bJumpHeld && CanJump())
		{
			MovementComponent->TryJump();
		}
	}
	else if (PrevMovementMode == EMovementMode::MOVE_Walking)
	{
		MovementComponent->TryDashStop();
		UnCrouch();
	}
}


void AFightingGameCharacter::JumpStart()
{
	ProcessButton("Up");

	bJumpHeld = true;
	MovementComponent->TryJump();
}

void AFightingGameCharacter::JumpStop()
{
	bJumpHeld = false;
}

bool AFightingGameCharacter::CanJump() const
{
	return CurrentState == CS_Default && GetCharacterMovement()->IsWalking();
}

bool AFightingGameCharacter::CanAirJump() const
{
	return CurrentState == CS_Default && GetCharacterMovement()->IsFalling();
}

//void AFightingGameCharacter::Jump()
//{
//	AirActions = 1;
//
//	if (FMath::Abs(ForwardInput) < 0.5f)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Neutral Jump Executed"));
//		Super::Jump();
//	}
//	else // Directional Jump
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Directional Jump Executed"));
//		FVector JumpVector;
//		if (FMath::Abs(GetCharacterMovement()->Velocity.X) > 300.f)
//		{
//			JumpVector = FVector(JumpXVelocity/2 * FMath::Sign(ForwardInput), 0.f, GetCharacterMovement()->JumpZVelocity);
//			LaunchCharacter(JumpVector, false, true);
//		}
//		else // ensure minimum X velocity (bXYOverride = true)
//		{
//			JumpVector = FVector(JumpXVelocity * FMath::Sign(ForwardInput), 0.f, GetCharacterMovement()->JumpZVelocity);
//			LaunchCharacter(JumpVector, true, true);
//		}
//	}
//}

//void AFightingGameCharacter::DoubleJump()
//{
//	AirActions--;
//
//	FVector JumpVector = FVector(DoubleJumpXVelocity * FMath::Sign(ForwardInput), 0.f, DoubleJumpZVelocity);
//	LaunchCharacter(JumpVector, true, true);
//}

void AFightingGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//AirActions = 1;
	UE_LOG(LogTemp, Warning, TEXT("Landed"));

	if (CurrentState == CS_Attacking)
	{
		StopAnimMontage(AttackMontage);
		StopAttack();
	}
	else if (CurrentState == CS_Stunned)
	{
		PlayAnimMontage(HitMontage, 1.f, "Land");
	}
}


void AFightingGameCharacter::CrouchStart()
{
	bCrouchHeld = true;

	// Actual Crouch Logic
	if (CurrentState == CS_Default && GetCharacterMovement()->IsWalking())
	{	
		OnCrouch();
	}

	// Register the input for our Input Buffer
	FVector CharacterDirection = (OtherCharacter->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
	if ((ForwardInput > 0.3f && FVector::DotProduct(CharacterDirection, FVector::ForwardVector) > 0.3f) ||
		(ForwardInput < -0.3f && FVector::DotProduct(CharacterDirection, -FVector::ForwardVector) > 0.3f))
	{
		ProcessButton("DownForward");

		bDiagonal = true;
	}
	else if ((ForwardInput < -0.3f && FVector::DotProduct(CharacterDirection, FVector::ForwardVector) > 0.3f) ||
		(ForwardInput > 0.3f && FVector::DotProduct(CharacterDirection, -FVector::ForwardVector) > 0.3f))
	{
		ProcessButton("DownBack");

		bDiagonal = true;
	}
	else
	{
		ProcessButton("Down");
	}
}

void AFightingGameCharacter::OnCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("OnCrouch"));

	MovementComponent->TryDashStop();

	if (!bIsCrouched)
	{
		Crouch();

		FVector CrouchedExtent = FVector(Hurtbox->GetScaledBoxExtent().X, Hurtbox->GetScaledBoxExtent().Y, 65.f);
		Hbox->SetBoxExtent(CrouchingHurtboxExtent, true);
		Hbox->AddRelativeLocation(CrouchingHurtboxOffset);
	}
}

void AFightingGameCharacter::CrouchStop()
{
	bCrouchHeld = false;

	// Actual Crouch stop logic
	if (CurrentState == CS_Default && GetCharacterMovement()->IsWalking())
	{
		OnCrouchStop();
	}


	// Let go of down, while still holding Forward/Back
	// Input buffer recognizes the input independently
	FVector CharacterDirection = (OtherCharacter->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
	if ((ForwardInput > 0.3f && FVector::DotProduct(CharacterDirection, FVector::ForwardVector) > 0.3f) ||
		(ForwardInput < -0.3f && FVector::DotProduct(CharacterDirection, -FVector::ForwardVector) > 0.3f))
	{
		ProcessButton("Forward");
	}
	else if ((ForwardInput < -0.3f && FVector::DotProduct(CharacterDirection, FVector::ForwardVector) > 0.3f) ||
		(ForwardInput > 0.3f && FVector::DotProduct(CharacterDirection, -FVector::ForwardVector) > 0.3f))
	{
		ProcessButton("Back");
	}
}

void AFightingGameCharacter::OnCrouchStop()
{
	UE_LOG(LogTemp, Warning, TEXT("OnCrouchStop"));

	if (bIsCrouched)
	{
		UnCrouch();

		FVector StandingExtent = FVector(Hurtbox->GetScaledBoxExtent().X, Hurtbox->GetScaledBoxExtent().Y, 90.f);
		Hbox->SetBoxExtent(StandingHurtboxExtent, true);
		Hbox->AddRelativeLocation(-CrouchingHurtboxOffset);
	}
}



void AFightingGameCharacter::TryDash()
{
	if (CurrentState == CS_Default)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			if ((ForwardInput > 0.3 && IsOtherCharacterLeft()) ||
				(ForwardInput < -0.3 && !IsOtherCharacterLeft()))
			{
				MovementComponent->TryAirDash();
			}
			else if ((ForwardInput > 0.3 && !IsOtherCharacterLeft()) ||
				(ForwardInput < -0.3 && IsOtherCharacterLeft()))
			{
				MovementComponent->TryAirDashBack();
			}
		}
		else if (GetCharacterMovement()->IsWalking())
		{
			if ((ForwardInput > 0.3 && IsOtherCharacterLeft()) ||
				(ForwardInput < -0.3 && !IsOtherCharacterLeft()))
			{
				MovementComponent->TryDash();
			}
			else if ((ForwardInput > 0.3 && !IsOtherCharacterLeft()) ||
				(ForwardInput < -0.3 && IsOtherCharacterLeft()))
			{
				MovementComponent->TryBackDash();
			}
		}
	}
}

void AFightingGameCharacter::TryDashStop()
{
	if (CurrentState == CS_Default && MovementComponent->IsDashing())
	{
		MovementComponent->TryDashStop();
	}
}


//
//void AFightingGameCharacter::DashStart()
//{
//	if (CurrentState == CS_Default && GetCharacterMovement()->IsWalking())
//	{
//
//		GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
//		GetCharacterMovement()->Velocity = GetActorForwardVector() * DashSpeed;
//		bIsDashing = true;
//	}
//}
//
//
//void AFightingGameCharacter::DashStop()
//{
//	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
//	bIsDashing = false;
//}
//
//void AFightingGameCharacter::BackDash()
//{
//	if (CurrentState == CS_Default && GetCharacterMovement()->IsWalking())
//	{
//		CurrentState = CS_Dashing;
//		PlayAnimMontage(MoveMontage, 1.f, "BackDash");
//	}
//}
//
//void AFightingGameCharacter::AirDash()
//{
//	if (CurrentState == CS_Default && GetCharacterMovement()->IsFalling() && GetActorLocation().Z > 160.f /*&& AirActions > 0*/)
//	{
//		//AirActions--;
//
//		PlayAnimMontage(MoveMontage, 1.f, "AirDash");
//		CurrentState = CS_Dashing;
//		GetCharacterMovement()->GravityScale = 0.f;
//		GetCharacterMovement()->Velocity = FVector::ZeroVector;
//		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
//	}
//}
//
//void AFightingGameCharacter::AirDashBack()
//{
//	if (CurrentState == CS_Default && GetCharacterMovement()->IsFalling() && GetActorLocation().Z > 160.f /*&& AirActions > 0*/)
//	{
//		//AirActions--;
//
//		PlayAnimMontage(MoveMontage, 1.f, "AirDashBack");
//		CurrentState = CS_Dashing;
//		GetCharacterMovement()->GravityScale = 0.f;
//		GetCharacterMovement()->Velocity = FVector::ZeroVector;
//		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
//	}
//}
//
//void AFightingGameCharacter::AirDashLaunch()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Launch"));
//
//	//FVector LaunchVector = FVector(1.f, 1.f, 0.f) * AirDashSpeed;
//	//LaunchCharacter(LaunchVector, true, true);
//}
//
//void AFightingGameCharacter::AirDashStop()
//{
//	StopAnimMontage(MoveMontage);
//	CurrentState = CS_Default;
//	GetCharacterMovement()->GravityScale = DefaultGravity;
//	GetCharacterMovement()->Velocity = FVector::ZeroVector;
//	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
//}



void AFightingGameCharacter::ProcessForwardInput(float value)
{
	if (bWasNeutral)
	{
		bWasNeutral = false;

		//FVector CharacterDirection = (OtherCharacter->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
		if ((value > 0.3f && IsCharacterFacingLeft()) ||
			(value < -0.3f && !IsCharacterFacingLeft()))
		{
			if (bCrouchHeld)
			{
				ProcessButton("DownForward");
			}
			else
			{
				ProcessButton("Forward");
				UE_LOG(LogTemp, Warning, TEXT("Forward Press"));
			}
		}
		else if ((value < -0.3f && IsCharacterFacingLeft()) ||
				 (value > 0.3f && !IsCharacterFacingLeft()))
		{
			if (bCrouchHeld)
			{
				ProcessButton("DownBack");
			}
			else
			{
				ProcessButton("Back");
				UE_LOG(LogTemp, Warning, TEXT("Back Press"));
			}
		}
	}


	if (value < 0.3f && value > -0.3f)
	{
		TryDashStop();

		bWasNeutral = true;
		if (bDiagonal == true)
		{
			bDiagonal = false;

			ProcessButton("Down");
		}
	}
}

void AFightingGameCharacter::ProcessButton(FString inputName)
{
	FInputData Data;
	Data.inputName = inputName;
	Data.timePassed = GetWorld()->GetTimeSeconds();
	AddToInputBuffer(Data);
}


void AFightingGameCharacter::AddToInputBuffer(FInputData inputData)
{
	InputToAdd = inputData;
	UpdateInputs();

	/*ShowInputOnScreen(inputData);

	InputBuffer.Add(inputData);
	CheckForCommands();*/
}

void AFightingGameCharacter::FinishAddToInputBuffer()
{
	ShowInputOnScreen(InputToAdd);

	InputBuffer.Add(InputToAdd);
	CheckForCommands();
}

void AFightingGameCharacter::CheckForCommands()
{
	FCommand CurrentCommand;
	CurrentCommand.name = "Invalid";
	CurrentCommand.priority = 20;

	for (FCommand Command : CommandList)
	{
		if (CheckBufferForCommand(Command) && Command.priority < CurrentCommand.priority)
		{
			CurrentCommand = Command;
		}
	}

	if (CurrentCommand.name != "Invalid")
	{
		TryCommand(CurrentCommand);
	}
}

bool AFightingGameCharacter::CheckBufferForCommand(FCommand Command)
{
	int SequenceNum = 0;


	for (int i = 0; i < InputBuffer.Num(); i++)
	{
		if (Command.inputs[SequenceNum] == InputBuffer[i].inputName)
		{
			SequenceNum++;
			if (SequenceNum == Command.inputs.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("Command: %s, Succeeded"), *Command.name)

				return true;
			}
		}
	}

	return false;
}

void AFightingGameCharacter::TryCommand(FCommand Command)
{
	if (bInComboWindow && (SpecialCancelable(Command) && CheckIfHit()) ||
		(CurrentState == CS_Default))
	{

		UE_LOG(LogTemp, Warning, TEXT("Try Command: %s"), *Command.name);

		if (Command.name == "Dash")
		{
			if (GetCharacterMovement()->IsFalling())
			{
				MovementComponent->TryAirDash();
			}
			else
			{
				MovementComponent->TryDash();
			}
		}
		else if (Command.name == "BackDash")
		{
			if (GetCharacterMovement()->IsFalling())
			{
				MovementComponent->TryAirDashBack();
			}
			else
			{
				MovementComponent->TryBackDash();
			}
		}
		else if ((GetMovementComponent()->IsFalling() && Command.AirOK) ||
			(!GetMovementComponent()->IsFalling() && Command.GroundOK))
		{
			CurrentState = CS_Attacking;
			PlayAnimMontage(SpecialMontage, 1.f, *Command.name);
			InputBuffer.Empty(); // not done on dash in case a special is input immediately after
		}
	}
}

void AFightingGameCharacter::FireProjectile()
{
	FVector Offset = GetActorLocation() + FVector(0.f, 0.f, 30.f);

	AProjectile* Projectile = Cast<AProjectile>(GetWorld()->SpawnActor<AProjectile>(ProjectileClass,Offset + (100.f * GetActorForwardVector()), GetActorRotation()));
	Projectile->SetOwner(this);
	Projectile->FireInDirection(GetActorForwardVector());
}

void AFightingGameCharacter::StartInvincibility()
{
	bHitInvulnerable = true;
	Hbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Hbox->SetVisibility(false);
}

void AFightingGameCharacter::StopInvincibility()
{
	bHitInvulnerable = false;
	Hbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hbox->SetVisibility(true);
}

void AFightingGameCharacter::SetCharacterHeight(float NewHeight)
{
	// GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight); // doesnt work alone
}

void AFightingGameCharacter::ResetCharacterHeight()
{
	if (bIsCrouched)
	{
		//GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchedEyeHeight);
	}
	else
	{
		//GetCapsuleComponent()->SetCapsuleHalfHeight(GetDefaultHalfHeight());
	}
}



void AFightingGameCharacter::UpdateComboWidget()
{
	AFGHUD* FGHUD = Cast<AFGHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (FGHUD)
	{
		if (GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FGHUD->P1UpdateComboCount(ComboNum);
		}
		else if (GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 1))
		{
			FGHUD->P2UpdateComboCount(ComboNum);
		}
	}
}

void AFightingGameCharacter::ResetComboWidget()
{
	AFGHUD* FGHUD = Cast<AFGHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (FGHUD)
	{
		if (GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FGHUD->P1ResetCombo();
		}
		else if (GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 1))
		{
			FGHUD->P2ResetCombo();
		}
	}
}