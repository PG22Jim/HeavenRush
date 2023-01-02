// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"

#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UParkourComponent::UParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


void UParkourComponent::Initialization(ACharacter* Player)
{
	PlayerCharacter = Player;
	PlayerMovement = PlayerCharacter->GetCharacterMovement();

	if(PlayerMovement == nullptr) return;
	
	// Grab values from Player movement component for default values
	DefaultGravity = PlayerMovement->GravityScale;
	DefaultGroundFriction = PlayerMovement->GroundFriction;
	DefaultBrakingDecelerationWaling = PlayerMovement->BrakingDecelerationWalking;
	DefaultMaxSpeed = PlayerMovement->MaxWalkSpeed;
	DefaultMaxSpeedCrouch = PlayerMovement->MaxWalkSpeedCrouched;

	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	World->GetTimerManager().SetTimer(MainLoopTimerHandle,this, &UParkourComponent::ParkourChecking, TimerSecond, true, 0);
}

void UParkourComponent::ParkourJump()
{
	if(CurrentParkourMovement == Running)
	{
		if(PlayerMovement->IsFalling())
			OpenAllGates();

		return;
	}

	WallRunJump();
	LedgeGrabJump();
	SlideJump();
	CrouchJump();
	SprintJump();
}

void UParkourComponent::Landing()
{
	EndAllParkourEvents();
	CloseAllGates();
	ClimbDurationCounter = 0;
	WallClimbCoolDown();
}

void UParkourComponent::MovementChange(EMovementMode PreviousMode, EMovementMode NewMode)
{
	if(PlayerMovement == nullptr) return;

	PreviousMovementMode = PreviousMode;
	CurrentMovementMode = NewMode;
	
	// if player's movement is switching from walking to falling
	if(PreviousMovementMode == MOVE_Walking && CurrentMovementMode == MOVE_Falling)
	{
		SprintJump();
		EndAllParkourEvents();
		OpenAllGates();
	}
	else if (PreviousMovementMode == MOVE_Falling && CurrentMovementMode == MOVE_Walking)
	{
		CheckQueues();
	}
}

void UParkourComponent::SprintCheck()
{
	if(CurrentParkourMovement == Running) StartSprint();
}

void UParkourComponent::TryCrouch()
{
	
}

void UParkourComponent::TryDash()
{
	// player cannot do dashing while dashing
	if(IsDashing) return;

	// Reference check
	if(PlayerCharacter == nullptr) return;
	if(PlayerMovement == nullptr) return;

	// flip IsDashing to prevent multiple dashing at once
	IsDashing = true;
	
	// Reset velocity
	PlayerMovement->Velocity = {0,0,0};

	// Sphere trace by channel to check if there are something from start to dashing destination
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	DashingDestination = FindDashDestination(PlayerLocation);
	if(DashingDestination == FVector {0,0,0}) return;
	
	if(PlayerCharacter->CustomTimeDilation <= 0.5)
	{
		const float PlayerCurrentSpeedRate = PlayerCharacter->CustomTimeDilation;
		const float SpeedMultiplier = 1 / PlayerCurrentSpeedRate;
		DashingTimeline.SetPlayRate(SpeedMultiplier);
	}

	else
	{
		DashingTimeline.SetPlayRate(1);
	}
	
	DashingTimeline.PlayFromStart();
}

void UParkourComponent::TryStopDashing()
{
	if(!DashingTimeline.IsPlaying()) return;
	
	DashingTimeline.Stop();
	IsDashing = false;
}


// Called when the game starts
void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	// Dash Timeline initialization
	FOnTimelineFloat DashProgressUpdate;
	DashProgressUpdate.BindDynamic(this, &UParkourComponent::DashUpdate);

	FOnTimelineEvent DashProgressFinish;
	DashProgressFinish.BindDynamic(this, &UParkourComponent::DashFinish);

	DashingTimeline.AddInterpFloat(DashTimelineCurve, DashProgressUpdate);
	DashingTimeline.SetTimelineFinishedFunc(DashProgressFinish);
}

// Main Looping Function
void UParkourComponent::ParkourChecking()
{
	// switch (CurrentParkourMovement)
	// {
	// case Running:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Running"));
	// 	break;
	// case LeftWallRun:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("LeftWallRun"));
	// 	break;
	// case RightWallRun:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("RightWallRun"));
	// 	break;
	// case EParkourMovementType::WallClimb:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("WallClimb"));
	// 	break;
	// case LedgeGrab:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("LedgeGrab"));
	// 	break;
	// case Mantle:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Mantle"));
	// 	break;
	// case EParkourMovementType::Slide:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Slide"));
	// 	break;
	// case Crouch:
	// 	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Crouch"));
	// 	break;
	// default:
	// 	break;
	// }

	
	if(WallRunGate.IsOpen())
		HorizontalWallRunning();

	if(DebugCanWallClimb)
		if(WallClimbGate.IsOpen())
			WallClimb();

	MantleStart();
	if(SprintGate.IsOpen()) SprintUpdate();
	if(SlideGate.IsOpen()) Slide();
	CameraTick();
}

// ===========================================  HORIZONTAL WALL RUNNING  ===========================================
// =================================================================================================================
#pragma region WallRunning 
void UParkourComponent::HorizontalWallRunning()
{
	// if conditions are not matched, end wall running
	if(!WallRunCheck())
	{
		WallRunEnd(1.0f);
		return;
	}


	// Set parkour movement to right wall run and interp gravity scale
	const FVector RightEndVec = GetRightWallRunVec();
	if(WallRunMovement(PlayerCharacter->GetActorLocation(),RightEndVec,-1.0))
	{
		const bool bMovementChange = SetParkourMovement(EParkourMovementType::RightWallRun);
		SetGravityScale();

		if(bMovementChange)
		{
			APlayerCharacter* ThePlayerCharacterClass = Cast<APlayerCharacter>(PlayerCharacter);
			if(ThePlayerCharacterClass != nullptr) ThePlayerCharacterClass->StartWallRunSound(false);
		}
	}
	else
	{
		// if player's condition is right wall run but there is no hit result, there is no wall for player to do right wall run
		if(CurrentParkourMovement == EParkourMovementType::RightWallRun) 
		{
			WallRunEnd(0.5);
			return;
		}
		// if player is not right wall running but result is true, player is left wall running
		if (WallRunMovement(PlayerCharacter->GetActorLocation(), GetLeftWallRunVec(), 1.0))
		{
			const bool bMovementChange = SetParkourMovement(EParkourMovementType::LeftWallRun);
			SetGravityScale();
			if(bMovementChange)
			{
				APlayerCharacter* ThePlayerCharacterClass = Cast<APlayerCharacter>(PlayerCharacter);
				if(ThePlayerCharacterClass != nullptr) ThePlayerCharacterClass->StartWallRunSound(true);
			}
		}
		else WallRunEnd(0.5);
	}
}

bool UParkourComponent::WallRunCheck() const
{
	// if these conditions are true, return true
	return IsForwardInput()&&(CurrentParkourMovement==EParkourMovementType::Running || IsPlayerWallRunning());
}

bool UParkourComponent::WallRunMovement(FVector StartPos, FVector EndPos, float Direction)
{
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	const TArray<AActor*> IgnoreActors;
	// Line trace by channel from Kismet System Library 
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartPos, EndPos, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Hit,true);

	// if there is no result, no wall is beside player
	if(!bHit) return false;

	// store hit result's normal vector
	WallRunNormal = Hit.Normal;
	// check if normal vector is valid and player is in mid air
	const bool bResult = IsValidWallVector(WallRunNormal) && PlayerMovement->IsFalling();

	if(!bResult) return false;

	//calculate 
	const FVector WallRunNormalized = UKismetMathLibrary::Cross_VectorVector(WallRunNormal,{0,0,1.0});

	// calculate launched velocity
	const FVector LaunchedVelocity = TryDecreaseWallRunSpeed(WallRunNormalized, Direction);

	
	// launch player to keep player on wall
	PlayerCharacter->LaunchCharacter(LaunchedVelocity,true, true);
	
	return true;
}

bool UParkourComponent::IsPlayerWallRunning() const
{
	// return true if player is wall running left or right
	return (CurrentParkourMovement == EParkourMovementType::LeftWallRun || CurrentParkourMovement == EParkourMovementType::RightWallRun);
}

bool UParkourComponent::IsValidWallVector(FVector HitNormalVector)
{
	// return true if trace hit normal is in range
	return UKismetMathLibrary::InRange_FloatFloat(HitNormalVector.Z,-0.52,0.52);
}

void UParkourComponent::WallRunEnd(float ResetTime)
{
	if(!IsPlayerWallRunning()) return;
	
	const bool bChanged = SetParkourMovement(EParkourMovementType::Running);

	if(!bChanged) return;

	CloseWallRunGate();

	const UWorld* World = GetWorld();
	if(World == nullptr) return;


	
	World->GetTimerManager().SetTimer(WallRunGateTimerHandle,this, &UParkourComponent::OpenWallRunGate, ResetTime, false, -1);
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Timer Set"));
	
}

void UParkourComponent::OpenWallRunGate()
{
	WallRunGate.Open();
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Function Called"));
}

void UParkourComponent::CloseWallRunGate()
{
	WallRunGate.Close();
}

FVector UParkourComponent::TryDecreaseWallRunSpeed(FVector WallRunNormalized, float Direction)
{
	// If player get slowed, decrease wall run speed
	if(IsSlowed) WallRunSpeed = 300.0f;
	else WallRunSpeed = 1100.0f;

	const FVector LaunchVelocity = WallRunNormalized * (WallRunSpeed * Direction); 
	// return Velocity for launching character
	return  LaunchVelocity;
}
#pragma endregion 
// =================================================================================================================



// =============================================  WALL CLIMBING  ===================================================
// =================================================================================================================
#pragma region WallClimbing
void UParkourComponent::WallClimb()
{
	// Check if player character and movement reference is nullptr
	if(PlayerCharacter == nullptr || PlayerMovement == nullptr) return;

	
	// if condition not match, end wall climbing
	if(!WallClimbCheck())
	{
		WallClimbEnd(0.35);
		return;
	}

	
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	const TArray<AActor*> IgnoreActors;

	// grab positions from player's view height and player's feet position
	const FVector Eye = GetEyePos();
	const FVector Feet = GetFeetPos();

	// Capsule trace by channel
	const bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(this, Eye, Feet, 20.0f, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);

	// override old MantleTraceDistance with distance from trace start pos to hit point 
	MantleTraceDistance = Hit.Distance;
	
	// if not able to mantle, continue climbing
	if(!bHit || !PlayerMovement->IsWalkable(Hit))
	{
		WallClimbMovement();
		return;
	}


	// if player is not walkable, player is still climbing on the wall

	
	// If the trace part is walkable for player, it means player can Ledge grab
	const FVector PlusVec = {0,0,PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()};
	MantlePos = Hit.ImpactPoint + PlusVec;

	CloseWallClimbGate();

	if(SetParkourMovement(LedgeGrab))
	{
		PlayerMovement->DisableMovement();
		PlayerMovement->StopMovementImmediately();
		PlayerMovement->GravityScale = 0.0f;
		if(IsQuickMantle()) MantleGate.Open();
		else
		{
			const UWorld* World = GetWorld();
			if(World == nullptr) return;

			World->GetTimerManager().SetTimer(MantleCheckGateTimerHandle,this, &UParkourComponent::OpenMantleCheckGate, 0.05, false, -1.0f);
			//World->GetTimerManager().SetTimer(MantleCheckGateTimerHandle,this, &UParkourComponent::OpenMantleCheckGate, 0.25, false, 0.5f);
		}
	}
	
	
}

bool UParkourComponent::WallClimbCheck() const
{
	// return true if player is moving forward, player is falling, ParkourMovementType is Running, player is horizontally wall running, and bool is true
	const bool bPlayerForward = IsForwardInput();
	const bool bPlayerInMidair = PlayerMovement->IsFalling();
	const bool bPlayerRunning = CurrentParkourMovement == Running;
	const bool bWallRunning = IsPlayerWallRunning();
	
	const bool bIsReady = CanWallClimb;
	return bPlayerForward && bPlayerInMidair && (bPlayerRunning || bWallRunning) && bIsReady;
}

void UParkourComponent::ClimbCoolDownCheck()
{
	// if bool is already true, no need to count for cooldown time
	if(CanWallClimb) return;

	// timer increase by timer tick second and check if timer reaches the cooldown requirement
	const float Result = ClimbCoolDownCounter + TimerSecond;
	if(Result >= ClimbCoolDown)
	{
		// cooldown finished, reset timer and flip bool to true
		ClimbCoolDownCounter = 0.0f;
		CanWallClimb = true;
		return;
	}
	// continue in cooldown state
	ClimbCoolDownCounter = Result;
}

FVector UParkourComponent::GetEyePos() const
{
	// Empty pos and rot
	FVector EyeLocation;
	FRotator EyeRotation;

	// safely get player's controller
	const AController* PlayerController = PlayerCharacter->GetController();
	if(!PlayerController) return {0,0,0};

	// grab Actor's eye view points
	PlayerController->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	// calculate and return the value of eye position
	const FVector AddingVec = {0,0,50.0f};
	const FVector ForwardVec = PlayerCharacter->GetActorForwardVector();
	return (EyeLocation + AddingVec) + (ForwardVec * 50);
}

FVector UParkourComponent::GetFeetPos() const
{
	if(!PlayerCharacter) return {0,0,0};

	const UCapsuleComponent* CapsuleComponent = PlayerCharacter->GetCapsuleComponent();
	if(!CapsuleComponent) return {0,0,0};

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	// calculate and return the value of feet position
	const float CapsuleHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight();
	const FVector MinusVector = {0,0, CapsuleHeight - MantleHeight};
	const FVector ForwardVec = PlayerCharacter->GetActorForwardVector();
	return (PlayerLocation - MinusVector) + (ForwardVec * 50.0f);
}

void UParkourComponent::WallClimbMovement()
{
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	const TArray<AActor*> IgnoreActors;
	
	const FVector Feet = GetFeetPos();
	const FVector PlayerPos = PlayerCharacter->GetActorLocation();
	const FVector ForwardVec = PlayerCharacter->GetActorForwardVector();
	const FVector EndPos = PlayerPos + (ForwardVec * 75.0f);

	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, PlayerPos, EndPos,UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);
	
	
	// if one of conditions is not matched, failed to climb wall
	if(IsForwardInput() && bHit)
	{
		// if player is moving forward and trace hit, wall climbing
		WallClimbNormal = Hit.ImpactNormal;
		bool bChanged = SetParkourMovement(EParkourMovementType::WallClimb);

		// if player get slowed, slow down speed
		if(IsSlowed)
		{
			WallClimbSpeed = DefaultWallClimbSpeed / SlowPenaltyMultiplier;
			WallClimbSpeedRatio = DefaultClimbSpeedRatio / SlowPenaltyMultiplier;
		}
		else
		{
			WallClimbSpeed = DefaultWallClimbSpeed;
			WallClimbSpeedRatio = DefaultClimbSpeedRatio;
		}

		// calculate velocity to launch player only up 
		const FVector LaunchVelocityXY = WallClimbNormal * -600;
		PlayerCharacter->LaunchCharacter({LaunchVelocityXY.X,LaunchVelocityXY.Y,WallClimbSpeed}, true, true);
		WallClimbLimitation();
	}

	WallClimbEnd(0.35);
}

void UParkourComponent::WallClimbLimitation()
{
	// restriction to limit player only able to wall run in few seconds
	const float Result = ClimbDurationCounter + ClimbDurationIncrement;
	
	if(Result >= ClimbLimitation)
	{
		CanWallClimb = false;
		ClimbDurationCounter = 0.0f;
	}
	else
		ClimbDurationCounter = Result;
}

void UParkourComponent::WallClimbCoolDown()
{
	CanWallClimb = true;
}

void UParkourComponent::WallClimbEnd(float ResetTime)
{
	if(CurrentParkourMovement == EParkourMovementType::LedgeGrab || CurrentParkourMovement == EParkourMovementType::WallClimb || CurrentParkourMovement == EParkourMovementType::Mantle)
	{
		const bool bChanged = SetParkourMovement(Running);

		if(!bChanged) return;

		CloseWallClimbGate();
		CloseMantleCheckGate();
		
		const UWorld* World = GetWorld();
		if(World == nullptr) return;

		World->GetTimerManager().SetTimer(WallClimbGateTimerHandler,this, &UParkourComponent::OpenWallClimbGate, ResetTime, false, -1);
		World->GetTimerManager().SetTimer(QueueCheckTimerHandle,this, &UParkourComponent::CheckQueues, 0.2, false, -1);
	}
}

void UParkourComponent::OpenWallClimbGate()
{
	WallClimbGate.Open();
}

void UParkourComponent::CloseWallClimbGate()
{
	MantleGate.Close();
	WallClimbGate.Close();
}
#pragma endregion
// =================================================================================================================



// ================================================  MANTLE  =======================================================
// =================================================================================================================
#pragma region Mantle
void UParkourComponent::MantleStart()
{
	// If gate is open, do mantle check
	if(MantleCheckGate.IsOpen())
		MantleCheck();

	// if mantle is open, do mantle movement
	if(MantleGate.IsOpen())
		MantleMovement();
}

void UParkourComponent::MantleCheck()
{
	// if player can't mantle, no need to open mantle gate
	if(!CanMantle()) return;

	// change parkour movement, if parkour movement is not changed, it means player is mantling
	const bool bChanged = SetParkourMovement(Mantle);
	if(!bChanged) return;

	// TODO: Camera shake
	// Player can mantle, close check gate since there is no need to check again
	MantleCheckGate.Close();
	// open mantle gate for player to execute mantle movement
	MantleGate.Open();
	
}

bool UParkourComponent::CanMantle() const
{
	// player can mantle if player is trying to move forward, Parkour Movement is ledge grab or player is performing quick mantle
	return (IsForwardInput() && (CurrentParkourMovement == EParkourMovementType::LedgeGrab || IsQuickMantle()));
}

bool UParkourComponent::IsQuickMantle() const
{
	// check if player's is really close to ledge, if true, no need to do mantle slowly
	if(PlayerCharacter == nullptr) return false;
	const float HalfHeight = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	return MantleTraceDistance > HalfHeight;
}

void UParkourComponent::MantleMovement()
{
	// References check
	if(PlayerCharacter == nullptr) return;
	
	// Grab Actor controller reference
	AController* PlayerBaseController = PlayerCharacter->GetController();
	if(PlayerBaseController == nullptr) return;

	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	// Change Player's control rotation
	// Grab current rotation
	const FRotator CurrentRotation = PlayerBaseController->GetControlRotation();

	// Find lookat Rotation
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector LookStart = {PlayerLocation.X, PlayerLocation.Y, 0.0f};
	const FVector LookTarget = {MantlePos.X, MantlePos.Y, 0.0f};
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(LookStart, LookTarget);
	float DeltaTime = World->GetDeltaSeconds();
	
	// use Rinterp To to find new rotation to rotate to 
	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 7.0f);

	PlayerBaseController->SetControlRotation(NewRotation);


	// Change Player's position while mantling
	// Update Player's position
	PlayerLocation = PlayerCharacter->GetActorLocation();
	DeltaTime = World->GetDeltaSeconds();
	float InterpSpeed;

	// if QuickMantle, player's mantle speed increase, otherwise remain normal
	if(IsQuickMantle())
		InterpSpeed = QuickMantleSpeed;
	else
		InterpSpeed = NormalMantleSpeed;

	
	// VInterpTo to Update player's location
	const FVector NewLocation = UKismetMathLibrary::VInterpTo(PlayerLocation, MantlePos, DeltaTime, InterpSpeed);
	PlayerCharacter->SetActorLocation(NewLocation);

	// check if remaining distance is close to edge, if true, no need to climb for now
	const float Distance = UKismetMathLibrary::Vector_Distance(PlayerCharacter->GetActorLocation(), MantlePos);
	if(Distance < 8.0) WallClimbEnd(0.5);
}

void UParkourComponent::OpenMantleCheckGate()
{
	MantleCheckGate.Open();
}

void UParkourComponent::CloseMantleCheckGate()
{
	MantleCheckGate.Close();
}
#pragma endregion
// =================================================================================================================


// =================================================  JUMP  ========================================================
// =================================================================================================================
#pragma region Jump
void UParkourComponent::WallRunJump()
{
	// if player is not wall running, no need to perform jump from wall run
	if(!IsPlayerWallRunning()) return;

	// end wall run
	WallRunEnd(0.35);
	
	if(PlayerCharacter == nullptr) return;

	// Launch player 
	FVector LaunchVelocity;
	LaunchVelocity.X = WallRunJumpForce * WallRunNormal.X; 
	LaunchVelocity.Y = WallRunJumpForce * WallRunNormal.Y;
	LaunchVelocity.Z = WallRunJumpHeight;
	
	PlayerCharacter->LaunchCharacter(LaunchVelocity, false, true);

	WallClimbResetting();
}

void UParkourComponent::WallClimbResetting()
{
	CanWallClimb = true;
	ClimbDurationCounter = 0;
}

void UParkourComponent::LedgeGrabJump()
{
	if(!CanLedgeJump()) return;

	WallClimbEnd(0.35);

	if(PlayerCharacter == nullptr) return;

	FVector LaunchVelocity;
	LaunchVelocity.X = WallRunJumpForce * WallRunNormal.X; 
	LaunchVelocity.Y = WallRunJumpForce * WallRunNormal.Y;
	LaunchVelocity.Z = WallRunJumpHeight;

	PlayerCharacter->LaunchCharacter(LaunchVelocity, false, true);
}

bool UParkourComponent::CanLedgeJump() const
{
	return CurrentParkourMovement == LedgeGrab || CurrentParkourMovement == EParkourMovementType::WallClimb || CurrentParkourMovement == Mantle;
}

void UParkourComponent::SlideJump()
{
	if(CurrentParkourMovement == EParkourMovementType::Slide) SlideEnd(false);
}

void UParkourComponent::CrouchJump()
{
	if(CurrentParkourMovement == Crouch) CrouchEnd();
}

void UParkourComponent::SprintJump()
{
	if(CurrentParkourMovement == Running)
	{
		SprintEnd();
		IsSprintQueued = true;
	}
}
#pragma endregion
// =================================================================================================================


// ===============================================  Sprinting  =====================================================
// =================================================================================================================
#pragma region Sprinting
void UParkourComponent::StartSprint()
{
	CrouchEnd();

	SlideEnd(false);
	
	if(CanSprint())
	{
		const bool bChanged = SetParkourMovement(Running);
		if(!IsSlowed) PlayerMovement->MaxWalkSpeed = SprintSpeed;

		OpenSprintGate();
		
		IsSlideQueued = false;
		IsSprintQueued = false;
	}
}

bool UParkourComponent::CanSprint() const
{
	if(PlayerMovement == nullptr) return false;
	return CurrentParkourMovement == Running && PlayerMovement->IsWalking();
}

void UParkourComponent::OpenSprintGate()
{
	SprintGate.Open();
}

void UParkourComponent::CloseSprintGate()
{
	SprintGate.Close();
}

void UParkourComponent::SprintUpdate()
{
	if(CurrentParkourMovement != Running) return;

	if(!IsForwardInput()) SprintEnd();
	
}

void UParkourComponent::SprintEnd()
{
	if(CurrentParkourMovement != Running) return;

	const bool bChanged = SetParkourMovement(Running);

	CloseSprintGate();

	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	World->GetTimerManager().SetTimer(SprintGateTimerHandler,this, &UParkourComponent::OpenSprintGate, 0.1, false, 0);
}
#pragma endregion
// =================================================================================================================


// ================================================  Crouch  =======================================================
// =================================================================================================================
#pragma region Crouch
void UParkourComponent::CrouchOrSlide()
{
	if(PlayerMovement == nullptr) return;
	
	const bool bContinue = CancelParkour();

	if(!bContinue) return;

	if(!CanSlide())
	{
		CrouchToggle();
		return;
	}

	if(PlayerMovement->IsWalking()) SlideStart();
	else
		IsSlideQueued = true;
}

bool UParkourComponent::CancelParkour()
{
	// if player is wall climbing, cancel and end wall climbing
	if(CurrentParkourMovement == Mantle || CurrentParkourMovement == LedgeGrab || CurrentParkourMovement == EParkourMovementType::WallClimb)
	{
		WallClimbEnd(0.5);
		return false;
	}

	// if player is not wall climbing but wall running, stop wall running
	else if(IsPlayerWallRunning())
	{
		WallRunEnd(0.5);
		return false;
	}

	return true;
}

bool UParkourComponent::CanSlide() const
{
	return IsForwardInput() && ( CurrentParkourMovement==Running ||  IsSprintQueued);
}

void UParkourComponent::CrouchToggle()
{
	switch (CurrentParkourMovement)
	{
	case Running:
		CrouchStart();		
		break;
	case Crouch:
		CrouchEnd();
		break;
	default:
		break;
	}
}

void UParkourComponent::CrouchStart()
{
	if(CurrentParkourMovement == Running)
	{
		IsSprintQueued = SetParkourMovement(Crouch);
		IsSlideQueued = false;
	}
}

void UParkourComponent::CrouchEnd()
{
	if(CurrentParkourMovement != Crouch) return;

	bool bChanged = SetParkourMovement(Running);

	IsSprintQueued = false;
	IsSlideQueued = false;
}
#pragma endregion 


// =================================================================================================================


// ================================================  SLIDING  ======================================================
// =================================================================================================================
#pragma region Sliding
void UParkourComponent::Slide()
{
	SlideUpdate();
}

void UParkourComponent::SlideUpdate()
{
	if(PlayerMovement == nullptr) return;

	const float VecLength = PlayerMovement->Velocity.Size();
	
	
	if(CurrentParkourMovement == EParkourMovementType::Slide && VecLength <= 35) SlideEnd(true);
}

void UParkourComponent::SlideEnd(bool bCrouch)
{
	if(CurrentParkourMovement != EParkourMovementType::Slide) return;
	
	EParkourMovementType NewParkourMode;

	if(bCrouch)
		NewParkourMode = Crouch;
	else
		NewParkourMode = Running;

	if(!SetParkourMovement(NewParkourMode)) return;

	CloseSlideGate();
}

void UParkourComponent::OpenSlideGate()
{
	SlideGate.Open();
}

void UParkourComponent::CloseSlideGate()
{
	SlideGate.Close();
}

void UParkourComponent::SlideStart()
{
	if(PlayerMovement == nullptr) return;
	
	if(!(CanSlide() && PlayerMovement->IsWalking())) return;

	SprintEnd();

	SetParkourMovement(EParkourMovementType::Slide);

	PlayerMovement->GroundFriction = 0.0f;
	PlayerMovement->BrakingDecelerationWalking = 0.0;
	PlayerMovement->MaxWalkSpeed = 0.0;

	FVector ForwardVec = PlayerMovement->Velocity;

	UKismetMathLibrary::Vector_Normalize(ForwardVec, 0.0001);

	PlayerMovement->SetPlaneConstraintFromVectors(ForwardVec, PlayerCharacter->GetActorUpVector());

	PlayerMovement->SetPlaneConstraintEnabled(true);

	const FVector SlideVector = FindSlideVec();

	if(SlideVector.Z <= 0.02)
	{
		const FVector Impulse = SlideVector * SlideImpulseAmount;
		PlayerMovement->AddImpulse(Impulse, false);
	}

	OpenSlideGate();

	IsSprintQueued = false;
	IsSlideQueued = false;
}

FVector UParkourComponent::FindSlideVec() const
{
	if(PlayerCharacter == nullptr) return {0,0,0};
	
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	const TArray<AActor*> IgnoreActors;

	const FVector StartPos = PlayerCharacter->GetActorLocation();

	const FVector Displacement = PlayerCharacter->GetActorUpVector() * -200.0;
	const FVector EndPos = StartPos + Displacement;
	
	// Line trace by channel from Kismet System Library 
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartPos, EndPos, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Hit,true);

	const FVector ImpactNormal = Hit.ImpactNormal;

	const FVector CrossVec = UKismetMathLibrary::Cross_VectorVector(ImpactNormal, PlayerCharacter->GetActorRightVector());

	return CrossVec * -1;
}
#pragma endregion
// =================================================================================================================


// ===============================================  CameraTilt  ====================================================
// =================================================================================================================
#pragma region CameraTilt
void UParkourComponent::CameraTick()
{
	switch (CurrentParkourMovement)
	{
		case Running:
			CameraTilt(0);
			break;
		case LeftWallRun:
			CameraTilt(15);
			break;
		case RightWallRun:
			CameraTilt(-15);
			break;
		case EParkourMovementType::WallClimb:
			CameraTilt(0);
			break;
		case LedgeGrab:
			CameraTilt(0);
			break;
		case Mantle:
			CameraTilt(0);
			break;
		case EParkourMovementType::Crouch:
			CameraTilt(0);
		break;
		case EParkourMovementType::Slide:
			CameraTilt(0);
		break;
		default:
			break;
	}
}

void UParkourComponent::CameraTilt(float TargetX)
{
	if(PlayerCharacter == nullptr) return;

	const UWorld* World = GetWorld();
	
	AController* PlayerController = PlayerCharacter->GetController();
	if(PlayerController == nullptr) return;

	FRotator ControlRotation = PlayerController->GetControlRotation();

	const FRotator TargetRotation = FRotator{ControlRotation.Pitch, ControlRotation.Yaw, TargetX};
	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(ControlRotation,TargetRotation, World->GetDeltaSeconds(),10);

	PlayerController->SetControlRotation(NewRotation);
}
#pragma endregion 
// =================================================================================================================


// ================================================  DASHING  ======================================================
// =================================================================================================================
#pragma region Dashing
FVector UParkourComponent::FindDashDestination(FVector StartPos) const
{
	if(PlayerCharacter == nullptr) return {0,0,0};
	
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	const TArray<AActor*> IgnoreActors;
	
	const FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
	const FVector Displacement = UKismetMathLibrary::Conv_RotatorToVector(AimRotation) * DashDistance;
	const FVector EndPos = StartPos + Displacement;

	const float CapsuleQuarterHeight = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2;

	
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, StartPos, EndPos, CapsuleQuarterHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Hit,true);

	if(bHit) return Hit.Location;

	return Hit.TraceEnd;
}

void UParkourComponent::DashUpdate(float Alpha)
{
	if(PlayerCharacter == nullptr) return;

	
	
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	const FVector Destination = UKismetMathLibrary::VLerp(PlayerLocation, DashingDestination, Alpha);

	const FRotator DestRotation = PlayerCharacter->GetActorRotation();
	
	const bool bTeleport = PlayerCharacter->TeleportTo(Destination, DestRotation);

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,FString::Printf(TEXT("%f"), Alpha));
}

void UParkourComponent::DashFinish()
{
	if(PlayerCharacter == nullptr || PlayerMovement == nullptr) return;
	
	const FVector ForwardVec = PlayerCharacter->GetActorForwardVector();

	PlayerMovement->Velocity = ForwardVec * SimulateVelocityValue;

	IsDashing = false;
}
#pragma endregion 
// =================================================================================================================


// ===========================================  UTILITY FUNCTIONS  =================================================
// =================================================================================================================
#pragma region Utility
bool UParkourComponent::IsForwardInput() const
{
	// Checking if player is still facing to the input vector
	if(PlayerCharacter == nullptr) return false;
	
	const FVector PlayerForwardVec = PlayerCharacter->GetActorForwardVector();
	const FVector LastInputVec = PlayerMovement->GetLastInputVector();
	return UKismetMathLibrary::Dot_VectorVector(PlayerForwardVec,LastInputVec) > 0 ;
}

bool UParkourComponent::SetParkourMovement(EParkourMovementType OverrideMode)
{
	if(CurrentParkourMovement == OverrideMode) return false;

	ParkourMovementChange(CurrentParkourMovement, OverrideMode);
	return true;
}

void UParkourComponent::ParkourMovementChange(EParkourMovementType PreviousMode, EParkourMovementType NewMode)
{
	// basic changing previous and current movement enum
	PreviousParkourMovement = PreviousMode;
	CurrentParkourMovement = NewMode;
	ResetMovement();
}

void UParkourComponent::ResetMovement() const
{
	if(!(CurrentParkourMovement == EParkourMovementType::Running || CurrentParkourMovement == EParkourMovementType::Crouch)) return;

	// Set player movement setting back to default setting
	PlayerMovement->GravityScale = DefaultGravity;
	PlayerMovement->GroundFriction = DefaultGroundFriction;
	PlayerMovement->BrakingDecelerationWalking = DefaultBrakingDecelerationWaling;
	PlayerMovement->MaxWalkSpeed = DefaultMaxSpeed;
	PlayerMovement->MaxWalkSpeedCrouched = DefaultMaxSpeedCrouch;
	PlayerMovement->SetPlaneConstraintEnabled(false);

	// Change movement mode based on parkour movement condition
	if(PreviousParkourMovement == EParkourMovementType::Running || PreviousParkourMovement == EParkourMovementType::Mantle || PreviousParkourMovement == EParkourMovementType::Slide || PreviousParkourMovement == EParkourMovementType::Crouch)
		PlayerMovement->SetMovementMode(EMovementMode::MOVE_Walking);
	else
	{
		PlayerMovement->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

void UParkourComponent::SetGravityScale() const
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	const float DeltaSeconds = World->GetDeltaSeconds();

	PlayerMovement->GravityScale = UKismetMathLibrary::FInterpTo(PlayerMovement->GravityScale, 0.25f, DeltaSeconds, 10.0);
}

void UParkourComponent::OpenAllGates()
{
	OpenWallRunGate();
	OpenWallClimbGate();
	OpenSlideGate();
	OpenSprintGate();
}

void UParkourComponent::CloseAllGates()
{
	CloseWallRunGate();
	CloseWallClimbGate();
	CloseSlideGate();
	CloseSprintGate();
}

void UParkourComponent::EndAllParkourEvents()
{
	WallRunEnd(0);
	WallClimbEnd(0);
	SprintEnd();
	SlideEnd(false);
}

void UParkourComponent::CheckQueues()
{
	if(IsSlideQueued) SlideStart();
	else if (IsSprintQueued) StartSprint();
}

FVector UParkourComponent::GetRightWallRunVec() const
{
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector RightVector = PlayerCharacter->GetActorRightVector() * 75.0f;
	const FVector ForwardVector = PlayerCharacter->GetActorForwardVector() * -35.0f;
	return PlayerLocation + RightVector + ForwardVector;
}

FVector UParkourComponent::GetLeftWallRunVec() const
{
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector LeftVector = PlayerCharacter->GetActorRightVector() * -75.0f;
	const FVector ForwardVector = PlayerCharacter->GetActorForwardVector() * -35.0f;
	return PlayerLocation + LeftVector + ForwardVector;
}
#pragma endregion 
// =================================================================================================================


// =============================================  EVENT TICK ==============================================
// Called every frame
void UParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Advancing timeline if timeline is playing
	DashingTimeline.TickTimeline(DeltaTime);
	// ...
}

