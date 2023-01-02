// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gate.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourComponent.generated.h"

class UCurveFloat;

UENUM()
enum EParkourMovementType
{
	Running,
	LeftWallRun,
	RightWallRun,
	WallClimb,
	LedgeGrab,
	Mantle,
	Slide,
	Crouch
};

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREAL_PROJECT_BASE_API UParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParkourComponent();

	// Callable functions to be called from player blueprint
	UFUNCTION(BlueprintCallable)
	void Initialization(ACharacter* Player);

	UFUNCTION(BlueprintCallable)
	void ParkourJump();

	UFUNCTION(BlueprintCallable)
	void Landing();
	
	UFUNCTION(BlueprintCallable)
	void MovementChange(EMovementMode MovementMode, EMovementMode NewMode);

	UFUNCTION(BlueprintCallable)
	void SprintCheck();

	UFUNCTION(BlueprintCallable)
	void TryCrouch();

	UFUNCTION(BlueprintCallable)
	void TryDash();

	UFUNCTION(BlueprintCallable)
	void TryStopDashing();

	// Timer Handle for calling parkour functions in time
	FTimerHandle MainLoopTimerHandle;
	FTimerHandle WallRunGateTimerHandle;
	FTimerHandle QueueCheckTimerHandle;
	FTimerHandle WallClimbCDTimerHandle;
	FTimerHandle MantleCheckGateTimerHandle;
	FTimerHandle WallClimbGateTimerHandler;
	FTimerHandle SprintGateTimerHandler;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Reference to player character and player movement, Add UPROPERTY to avoid getting garbage collected
	UPROPERTY()
	ACharacter* PlayerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* PlayerMovement;

	// Enum variable of Movement mode
	EParkourMovementType CurrentParkourMovement;
	EParkourMovementType PreviousParkourMovement;
	EMovementMode CurrentMovementMode;
	EMovementMode PreviousMovementMode;

	// Timelines
	FTimeline DashingTimeline;

	// Utility Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UtilityProperty)
	float TimerSecond = 0.01f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UtilityProperty)
	float SlowPenaltyMultiplier = 2.0f;

	// Default Setting Variables
	float DefaultGravity;
	float DefaultGroundFriction;
	float DefaultMaxSpeed;
	float DefaultMaxSpeedCrouch;
	float DefaultBrakingDecelerationWaling;

	// Variables used for wall running and wall climbing
	FVector WallRunNormal;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UtilityProperty)
	bool IsSlowed = false;
	
	float WallRunSpeed;

	// Wall Run Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallRunning)
	float WallRunSpeedMax = 1100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallRunning)
	float WallRunSpeedSlowed = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallRunning)
	float WallRunGravity = 25.0f;
	
	
	

	// Wall Climb Variables
	float MantleTraceDistance;
	FVector MantlePos;
	FVector WallClimbNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	bool DebugCanWallClimb = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	float MantleHeight = 40.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	bool CanWallClimb = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing, meta = (ClampMin = 0.2, ClampMax = 0.8))
	float DefaultClimbSpeedRatio = 0.4f;

	float WallClimbSpeedRatio;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	float DefaultWallClimbSpeed = 600;

	float WallClimbSpeed;

	float ClimbDurationCounter = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	float ClimbLimitation = 2.0;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	float ClimbDurationIncrement = 0.0167f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	float ClimbCoolDownCounter = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallClimbing)
	float ClimbCoolDown = 3.0f;

	
	// Mantle variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mantle)
	float QuickMantleSpeed = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mantle)
	float NormalMantleSpeed = 20.0f;


	// Jump variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ParkourJump)
	float WallRunJumpForce = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ParkourJump)
	float WallRunJumpHeight = 400.0f;

	
	// Sprint Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	float SprintSpeed = 1000.0f;

	bool IsSprintQueued = false;
	
	// Slide Variables
	bool IsSlideQueued = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slide)
	float SlideImpulseAmount = 600.0f;


	// Dash variables
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Dashing)
	bool IsDashing = false;

	bool IsPlayerTimeSlow = false;
	
	FVector DashingDestination;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dashing)
	float SimulateVelocityValue = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dashing)
	float DashDistance = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dashing)
	UCurveFloat* DashTimelineCurve;
	
private:
	// Total Check
	void ParkourChecking();

	// Horizontal Wall Run functions
	void HorizontalWallRunning();
	bool WallRunCheck() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsPlayerWallRunning() const;
	static bool IsValidWallVector(FVector HitNormalVector);
	bool WallRunMovement(FVector StartPos, FVector EndPos, float Direction);
	void WallRunEnd(float ResetTime);
	void OpenWallRunGate();
	void CloseWallRunGate();
	FVector TryDecreaseWallRunSpeed(FVector WallRunNormalized, float Direction);

	// Wall Climb functions
	void WallClimb();
	bool WallClimbCheck() const;
	void ClimbCoolDownCheck();
	FVector GetEyePos() const;
	FVector GetFeetPos() const;
	void WallClimbMovement();
	void WallClimbLimitation();
	void WallClimbCoolDown();
	void WallClimbEnd(float ResetTime);
	void OpenWallClimbGate();
	void CloseWallClimbGate();
	
	// Mantle functions
	void MantleStart();
	bool IsQuickMantle() const;
	void MantleCheck();
	bool CanMantle() const;
	void MantleMovement();
	void OpenMantleCheckGate();
	void CloseMantleCheckGate();

	// Jump functions
	void WallRunJump();
	void WallClimbResetting();
	void LedgeGrabJump();
	bool CanLedgeJump() const;
	void SlideJump();
	void CrouchJump();
	void SprintJump();

	// Landing functions

	
	// Sprint functions
	void StartSprint();
	bool CanSprint() const;
	void OpenSprintGate();
	void CloseSprintGate();
	void SprintUpdate();
	void SprintEnd();


	// Crouch functions
	void CrouchOrSlide();
	bool CancelParkour();
	bool CanSlide() const;
	void CrouchToggle();
	void CrouchStart();
	void CrouchEnd();


	// Slide functions
	void Slide();
	void SlideUpdate();
	void SlideEnd(bool bCrouch);
	void OpenSlideGate();
	void CloseSlideGate();
	void SlideStart();
	FVector FindSlideVec() const;

	// Camera functions
	void CameraTick();
	void CameraTilt(float TargetX);


	// Dashing functions
	FVector FindDashDestination(FVector StartPos) const;

	// Delegate functions to be set on Dashing Timeline
	UFUNCTION()
	void DashUpdate(float Alpha);

	UFUNCTION()
	void DashFinish();
	
	// Utility Functions
	bool IsForwardInput() const;
	bool SetParkourMovement(EParkourMovementType OverrideMode);
	void ParkourMovementChange(EParkourMovementType PreviousMode, EParkourMovementType NewMode);
	void ResetMovement() const;
	void SetGravityScale() const;
	void OpenAllGates();
	void CloseAllGates();
	void EndAllParkourEvents();
	void CheckQueues();
	
	
	FVector GetRightWallRunVec() const;
	FVector GetLeftWallRunVec() const;

	
	// Gates for controlling parkour conditions
	UPROPERTY(VisibleAnywhere)
	FGate WallClimbGate = FGate(true);

	UPROPERTY(VisibleAnywhere)
	FGate WallRunGate = FGate(true);

	UPROPERTY(VisibleAnywhere)
	FGate MantleGate = FGate(true);

	UPROPERTY(VisibleAnywhere)
	FGate MantleCheckGate = FGate(true);

	UPROPERTY(VisibleAnywhere)
	FGate SprintGate = FGate(true);

	UPROPERTY(VisibleAnywhere)
	FGate SlideGate = FGate(true);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
