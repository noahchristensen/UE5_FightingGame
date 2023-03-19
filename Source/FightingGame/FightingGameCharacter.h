// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Hitbox.h"

#include "FightingGameCharacter.generated.h"


UENUM(BlueprintType)
enum ECharacterState
{
	CS_Default		UMETA(DisplayName = "MOVING"),
	CS_Attacking	UMETA(DisplayName = "ATTACKING"),
	CS_Stunned		UMETA(DisplayName = "STUNNED"),
	CS_Blocking		UMETA(DisplayName = "BLOCKING"),
	CS_Dashing		UMETA(DisplayName = "DASHING")
};

UENUM(BlueprintType)
enum AttackType
{
	AT_NONE = 0 	UMETA(DisplayName = "NONE"),
	AT_StandingPunch = 1 	UMETA(DisplayName = "StandingPunch"),
	AT_StandingKick = 2 	UMETA(DisplayName = "StandingKick"),
	AT_CrouchingPunch = 3 	UMETA(DisplayName = "CrouchingPunch"),
	AT_CrouchingKick = 4 	UMETA(DisplayName = "CrouchingKick"),
	AT_JumpingPunch = 5 	UMETA(DisplayName = "JumpingPunch"),
	AT_JumpingKick = 6		UMETA(DisplayName = "JumpingKick"),
	AT_Sweep = 7			UMETA(DisplayName = "Sweep"),
	AT_Special1 = 8			UMETA(DisplayName = "Special1")
};

USTRUCT(BlueprintType)
struct FInputData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		FString inputName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		float timePassed;
};

USTRUCT(BlueprintType)
struct FCommand
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		int priority = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		TArray<FString> inputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		bool AirOK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		bool GroundOK;
};


struct FHitProperties;
class UHitBox;
class AProjectile;

UCLASS()
class FIGHTINGGAME_API AFightingGameCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UHitBox* SPunchHitbox;
	UPROPERTY(EditAnywhere)
		UHitBox* CPunchHitbox;
	UPROPERTY(EditAnywhere)
		UHitBox* SKickHitbox;
	UPROPERTY(EditAnywhere)
		UHitBox* CKickHitbox;
	UPROPERTY(EditAnywhere)
		UHitBox* SweepHitbox;
	UPROPERTY(EditAnywhere)
		UHitBox* Special1Hitbox;

public:
	// Sets default values for this character's properties
	AFightingGameCharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Ideally have an array of hurtboxes for more complexity
		UBoxComponent* Hurtbox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Ideally have an array of hurtboxes for more complexity
		UBoxComponent* Hbox;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	//virtual void Jump()	override;
	virtual void Landed(const FHitResult& Hit) override;

	virtual void CrouchStart();
	virtual void CrouchStop();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	inline void SetOtherCharacter(AFightingGameCharacter* Other) { OtherCharacter = Other; } // used to setup gamestate, (gamemode)

	inline void SetCharacterState(ECharacterState NewState) { CurrentState = NewState; };

	// UI
	UFUNCTION(BlueprintPure)
		inline float GetHealthPercent() { return Health / HealthMax; }
	UFUNCTION(BlueprintPure)
		bool IsCharacterFacingLeft();
	UFUNCTION(BlueprintPure)
		bool IsOtherCharacterLeft();

	UFUNCTION(BlueprintPure)
		inline bool IsCharacterDashing() { return bIsDashing; }

	UFUNCTION(BlueprintPure)
		inline TEnumAsByte<ECharacterState> GetCharacterState() { return CurrentState; }

	// Movement
	UFUNCTION(BlueprintPure)
		FORCEINLINE UFGCharacterMovementComponent* GetCustomCharacterMovement() const { return MovementComponent; }
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	inline float GetForwardInput() const { return ForwardInput; } // directional jump
	bool CanJump() const;
	bool CanAirJump() const;
	void OnCrouch();
	void OnCrouchStop();


	virtual float ProcessHit(float Damage, EGuardType GuardType, FHitProperties Properties); // public function called by other actors to read how the damage is handled by the victim(block, hit, etc)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void ReceiveHitStop(); // called by HitMontage to notify the hit is over

	void BlockStop();

	void StartHitstop(float HitstopDuration, float Dilation); // Time Dilation
	void StopHitstop();

	virtual void ActivateHitbox(AttackType HitboxID); // called by AttackMontage, Enables the hitbox to allow hitting characters
	virtual void DeactivateHitbox(AttackType HitboxID);
	void StopAttack();

	void ComboWindowStart(); // window in which input for a combo's continuation is accepted
	void ComboWindowStop();

	void FireProjectile();

	void StartInvincibility();
	void StopInvincibility();

	void SetCharacterHeight(float NewHeight);
	void ResetCharacterHeight();

	void ResetCharacter();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Character State")
		TEnumAsByte<ECharacterState> CurrentState;

	// Animation
	UPROPERTY(EditAnywhere, Category=Animation)
		UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
		UAnimMontage* SpecialMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
		UAnimMontage* HitMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
		UAnimMontage* BlockMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
		UAnimMontage* MoveMontage;

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	bool bIsOverlapping = false;

	/* Movement */
	UPROPERTY(Category = Characer, VisibleAnywhere, BlueprintReadOnly)
		UFGCharacterMovementComponent* MovementComponent;

	// Jump/UpbCrouchHeld
	/*UPROPERTY(EditAnywhere, Category="Character Movement: Jumping")
		float JumpXVelocity = 200.f;*/
	bool bJumpHeld = false;
	/*unsigned int AirActions = 1;*/
	/*UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping")
		float DoubleJumpZVelocity = 400.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping")
		float DoubleJumpXVelocity = 120.f;*/
	void JumpStart();
	void JumpStop();
	//void DoubleJump();

	// Crouch/Down
	bool bCrouchHeld = false;
	UPROPERTY(EditAnywhere, Category = "Hurtbox")
		FVector StandingHurtboxExtent = FVector(30.f, 30.f, 90.f);
	UPROPERTY(EditAnywhere, Category = "Hurtbox")
		FVector CrouchingHurtboxExtent = FVector(30.f, 30.f, 70.f);
	UPROPERTY(EditAnywhere, Category = "Hurtbox")
		FVector CrouchingHurtboxOffset = FVector(0.f, 0.f, -15.f);

	// Dash
	void TryDash();
	void TryDashStop();
	/*void DashStart();
	void DashStop();
	void BackDash();
	void AirDash();
	void AirDashBack();*/
	float WalkSpeed; // Set on BeginPlay
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dash")
		float DashSpeed = 400.f;
	UPROPERTY(EditAnywhere, Category = "Character Movement: Dash")
		float AirDashSpeed = 500.f;
	float DefaultGravity = 1.f;
	bool bIsDashing = false;
	/* Movement End */

	// Flipping
	void FlipCharacter();
	bool ShouldFlip();

	//Pushing
	void PushOtherCharacter(); // Called on tick, push character out of the way if too close
	void OverlapPush(); // Alternative of PushOtherCharacter, for when the characters are overlapping
	UPROPERTY(EditDefaultsOnly, Category = Pushing) // Should be based on other character
		float OverlapPushRate = 10.f;
	UPROPERTY(EditDefaultsOnly, Category=Pushing) // Should be based on other character
		float MinDistancePushX = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = Pushing)
		float MinDistancePushZ = 170.f;

	void RestartCharacters();
	FVector StartPosition;


	// Attack Logic
	void TryAttack(AttackType Attack); // Main attack function
	//Inputs
	void Grab();
	void Punch();
	void Kick();
	// Attack States
	TEnumAsByte<AttackType> CurrentAttack = AT_NONE;
	TEnumAsByte<AttackType> BufferedAttack = AT_NONE;
	// Gatlings - Chain Moves
	UPROPERTY(EditAnywhere, Category = Moves)
		TArray<TEnumAsByte<AttackType>> SPunchGatlings;
	UPROPERTY(EditAnywhere, Category = Moves)
		TArray<TEnumAsByte<AttackType>> CPunchGatlings;
	UPROPERTY(EditAnywhere, Category = Moves)
		TArray<TEnumAsByte<AttackType>> SKickGatlings;
	UPROPERTY(EditAnywhere, Category = Moves)
		TArray<TEnumAsByte<AttackType>> CKickGatlings;

	// Combo - Attacker
	bool MoveWithinGatlings(AttackType CheckMove); // checks if move can be combo'd into from current move
	bool SpecialCancelable(FCommand Command); // checks if the current move can be special cancelled
	const bool CheckIfHit();	// reads if attack successful hit opponent(for use in combos)
	bool bAttackHit = false;
	bool bInComboWindow = false;
	FTimerHandle ComboHandle;
	void UpdateComboWidget();
	void ResetComboWidget();
	// Combo - Victim
	int ComboNum = 0;


	// Damage
	void ReceiveHit(float DamageAmount, FHitProperties Properties);
	UPROPERTY(EditAnywhere, Category = "Character State", meta=(ClampMin=0))
		float HealthMax = 400.f;
	UPROPERTY(EditAnywhere, Category = "Character State")
		float Health = 400.f;
	bool bIsReceivingHit = false;
	FTimerHandle HitstopHandle;
	FTimerHandle HitstunHandle;

	// Blocking
	UPROPERTY(VisibleAnywhere,Category="Character State")
		TEnumAsByte<EGuardType> CurrentGuard;
	void Block(TEnumAsByte<EGuardType> GuardType);
	void SetGuard(float value);
	bool WasBlockSuccessful(EGuardType);

	// Invulnerability
	bool bHitInvulnerable = false;
	bool bIsBlocking = false;

	// Input Buffer
	float ForwardInput = 0.f;
	void ProcessForwardInput(float value); // Can be used for Up/Jump input if input is read from axis rather than action
	void ProcessButton(FString inputName);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FInputData> InputBuffer;
	void AddToInputBuffer(FInputData inputData);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateInputs();
	UFUNCTION(BlueprintCallable)
		void FinishAddToInputBuffer();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowInputOnScreen(FInputData inputData);
	FInputData InputToAdd;
	FTimerHandle InputHandle;
	float InputRemovalTime = 1.f;
	bool bWasNeutral = false; // Neutral = directional inputs
	bool bDiagonal = false; // Means input consisted of both a Down/Up and Left/Right Input

	// Commands/Specials
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moves)
		TArray<FCommand> CommandList;
	void CheckForCommands();
	bool CheckBufferForCommand(FCommand Command);
	void TryCommand(FCommand Command);

	// Projectile
	UPROPERTY(EditAnywhere)
		TSubclassOf<AProjectile> ProjectileClass;

private:
	AFightingGameCharacter* OtherCharacter;
};
