// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserEnemy.h"

#include "ChaserAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"

AChaserEnemy::AChaserEnemy()
{
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>("Timeline Comp");
}

void AChaserEnemy::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat FlyProgressUpdate;
	FlyProgressUpdate.BindDynamic(this, &AChaserEnemy::FlyTo);
	FlyTimeline.AddInterpFloat(FlyingTimelineCurve,FlyProgressUpdate);

	FOnTimelineEvent FlyProgressFinish;
	FlyProgressFinish.BindDynamic(this, &AChaserEnemy::FinishFlying);
	FlyTimeline.SetTimelineFinishedFunc(FlyProgressFinish);

	FOnTimelineFloat TeleportProgressUpdate;
	TeleportProgressUpdate.BindDynamic(this, &AChaserEnemy::TeleportUpdating);
	TeleportTimeline.AddInterpFloat(TeleportTimelineCurve,TeleportProgressUpdate);

	FOnTimelineEvent TeleportFinish;
	TeleportFinish.BindDynamic(this, &AChaserEnemy::Teleportation);
	TeleportTimeline.SetTimelineFinishedFunc(TeleportFinish);

	InintializeBTAndBBComponent();
}

void AChaserEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FlyTimeline.TickTimeline(DeltaSeconds);
	TeleportTimeline.TickTimeline(DeltaSeconds);
}

float AChaserEnemy::GetAdjustedFlySpeed()
{
	const float Distance = UKismetMathLibrary::Abs(UKismetMathLibrary::Vector_Distance(DestinationPos, GetActorLocation()));
	return FollowingSpeed / Distance;
}

void AChaserEnemy::Teleportation()
{

	USkeletalMeshComponent* CharacterMesh = GetMesh();
	CharacterMesh->SetScalarParameterValueOnMaterials("Dissolve", 2);
	
	ReSetEmissiveColor();

	if(TeleportPos != FVector{0,0,0}) this->TeleportTo(TeleportPos,{0,0,0}, false, true);
	
	ContinueBehaviourTree();

	IsCanBeAttacked = true;
}

void AChaserEnemy::InintializeBTAndBBComponent()
{
	AController* ChaserController = GetController();
	if(Controller == nullptr) return;
		
	AChaserAIController* ChaserAIController =  Cast<AChaserAIController>(ChaserController);
	if(ChaserAIController == nullptr) return;

	if(CurrentChaserController == nullptr) CurrentChaserController = ChaserAIController;
		
	if(BTComponent == nullptr) BTComponent = CurrentChaserController->GetBTComponent();

	if(BBComponent == nullptr && BTComponent != nullptr) BBComponent = BTComponent->GetBlackboardComponent();
}

void AChaserEnemy::StartComplexFlying(FVector MoveToPos , bool IsByPassing)
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	DestinationPos = MoveToPos;
	
	NormalSpeed = TravelDistancePerUpdate;

	if(IsByPassing)
	{
		NormalSpeed *= BypassSpeedMultiplier;
	}
	

	IsByPassingObject = IsByPassing;

	IsCharacterMoving = true;

	World->GetTimerManager().SetTimer(ComplexFlyTimerHandle,this, &AChaserEnemy::ComplexFlying, 0.02, true, -1);
}

void AChaserEnemy::ComplexFlying()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;


	// Distance Checking in normal flying movement
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);
	const FVector CurrentPlayerPos = PlayerCharacter->GetActorLocation();
	const float Distance = UKismetMathLibrary::Vector_Distance(CurrentPlayerPos, GetActorLocation());
	
	// if player is already close, no need to fly
	if(Distance <= 300)
	{
		// complete task
		World->GetTimerManager().ClearTimer(ComplexFlyTimerHandle);
		IsCharacterMoving = false;
		OnFinishFlyingTask.Execute(BTComponent,true, false);
		return;
	}

	// Update Speed
	const float CurrentFlyingSpeed = UpdateSpeed(); 
	
	// Updating Next tick destination
	FVector NextPos = {0,0,0};
	
	if(IsByPassingObject)
	{
		NextPos = UpdateNextPos(DestinationPos, CurrentFlyingSpeed);
	}
	else
	{
		// Get next moving position base on player character's position;
		NextPos = UpdateNextPos(CurrentPlayerPos, CurrentFlyingSpeed);

	}
	// Update Rotation
	const FRotator UpdatedRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NextPos);
	SetActorRotation(UpdatedRotation);

	// Object blocking checking
	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);
	if(!IsByPassingObject)
	{
	
		FVector Start = GetActorLocation();
		FVector DirectionWithoutHeight = GetActorForwardVector();
		DirectionWithoutHeight.Z = 0;
		float DetectionDistance = 400;
		//if(IsByPassingObject) DetectionDistance = 100;
		FVector Offset = DirectionWithoutHeight * DetectionDistance;
		FVector End = Start + Offset;
	
	
		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, Start, End, 35.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

		// if something on the way and the object is in front of player, stop timer and execute delegate finish task function
		if(bHit && Distance > DetectionDistance)
		{
			World->GetTimerManager().ClearTimer(ComplexFlyTimerHandle);
			IsCharacterMoving = false;
			OnFinishFlyingTask.Execute(BTComponent,true, true);
			return;
		}
	}

	// Move to
	//const bool MovingSuccess = TeleportTo(NextPos, GetActorRotation());
	const bool bMovingSuccess = TeleportTo(NextPos, GetActorRotation());

	// check if Chaser can go to player
	if(IsByPassingObject)
	{
		if(!IsVerticalByPass)
		{
			FHitResult ByPassCheckHit;
			
			const bool bCannotSeePlayer = UKismetSystemLibrary::SphereTraceSingle(World, GetActorLocation(), CurrentPlayerPos, 35.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, ByPassCheckHit, true);

			// If there is no object blocking between chaser and player, finish moving task
			if(!bCannotSeePlayer)
			{
				World->GetTimerManager().ClearTimer(ComplexFlyTimerHandle);
				IsCharacterMoving = false;
				OnFinishFlyingTask.Execute(BTComponent,true, false);
				return;
			}
			
		}
		
		float DistanceToDestination = UKismetMathLibrary::Vector_Distance(DestinationPos, GetActorLocation());
		if(DistanceToDestination <= 40)
		{
			// complete task
			World->GetTimerManager().ClearTimer(ComplexFlyTimerHandle);
			IsCharacterMoving = false;
			if(IsByPassingDoneOnce)
			{
				IsByPassingDoneOnce = false;
				OnFinishFlyingTask.Execute(BTComponent,true, false);
			}
			else
			{
				IsByPassingDoneOnce = true;
				OnFinishFlyingTask.Execute(BTComponent,true, true);
			}
			return;
		}
		
	}
	
	// if Chaser cannot move and it is in attack range, no need to move
	if(!bMovingSuccess && Distance <= 300)
	{
		// complete task
		World->GetTimerManager().ClearTimer(ComplexFlyTimerHandle);
		IsCharacterMoving = false;
		OnFinishFlyingTask.Execute(BTComponent,true, false);
	}
}

FVector AChaserEnemy::UpdateNextPos(FVector DestionationLocation, float CurrentSpeed)
{

	const FVector CurrentChaserPos = GetActorLocation();
	
	const FVector MovingDirection = UKismetMathLibrary::Normal((DestionationLocation - CurrentChaserPos));

	const FVector ReturnPos = CurrentChaserPos + (MovingDirection * CurrentSpeed);
	
	return ReturnPos;
}

float AChaserEnemy::UpdateSpeed()
{
	return NormalSpeed * CustomTimeDilation;
}

void AChaserEnemy::FinishBeginFlyingAnimation()
{
	OnFinishBeginAnimationTask.Execute(BTComponent);
}

// =========================================== Begin Fly Animation ===========================================




// ===========================================
void AChaserEnemy::SimpleFly(FVector MoveToPos , bool IsByPassing)
{
	
	AIPostion = GetActorLocation();
	DestinationPos = MoveToPos;

	const float SpeedBasedOnDistance = GetAdjustedFlySpeed();

	float FlySpeed = SpeedBasedOnDistance * CustomTimeDilation * FlySpeedMultiplier;

	IsByPassingObject = IsByPassing;
	
	if(IsByPassing)
	{
		FlySpeed *= BypassSpeedMultiplier;
	}
	
	FlyTimeline.SetPlayRate(FlySpeed);

	IsCharacterMoving = true;
	
	FlyTimeline.PlayFromStart();
}

void AChaserEnemy::StartTeleportingTo(FVector TeleportingPos)
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	TeleportPos = TeleportingPos;
	
	StopBehaviourTree();
	StopChaserMovement();

	FVector NewColor = {0,0,1};
	SetEmissiveColor(NewColor);

	IsCanBeAttacked = false;

	TeleportTimeline.PlayFromStart();
}

void AChaserEnemy::StopChaserMovement()
{
	IsCharacterMoving = false;
	FlyTimeline.Stop();

	if(BBComponent == nullptr) return;

	// Change bool setting from black board component
	const bool bIsStoped = BBComponent->GetValueAsBool("DoesGetStoped");
	if(bIsStoped) BBComponent->SetValueAsBool("DoesGetStoped",false);
	
	//OnFinishFlyingTask.Execute(BTComponent,true, false);
}

void AChaserEnemy::SetCanAttack()
{
	AController* ChaserController = GetController();
	if(Controller == nullptr) return;
	
	AChaserAIController* ChaserAIController = Cast<AChaserAIController>(ChaserController);
	if(ChaserAIController == nullptr) return;

	UBlackboardComponent* BlackBoard = ChaserAIController->GetBlackboardComponent();
	if(BlackBoard == nullptr) return;
	
	BlackBoard->SetValueAsEnum("CloseRangeCondition",1);
}


// ========================================================== Timeline Functions ==========================================================

void AChaserEnemy::ReceiveDamageEvent(float DecreasedDamage)
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	World->GetTimerManager().ClearTimer(ComplexFlyTimerHandle);
	IsCharacterMoving = false;

	if(BBComponent == nullptr) return;

	// Change bool setting from black board component
	const bool bIsStoped = BBComponent->GetValueAsBool("DoesGetStoped");
	if(bIsStoped) BBComponent->SetValueAsBool("DoesGetStoped",false);
	
	Super::ReceiveDamageEvent(DecreasedDamage);
}

void AChaserEnemy::FlyTo(float Alpha)
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);

	// Changing moving destination system

	// if chaser traveled over middle way point and it is following player, check if player is moving away 
	if(Alpha > FlyTimeline.GetTimelineLength()/2 && !IsByPassingObject)
	{
		const FVector PlayerLoc = PlayerCharacter->GetActorLocation();
		const float PlayerMovingDisplacement = UKismetMathLibrary::Vector_Distance(DestinationPos,PlayerLoc);

		// If player is constantly moving, chaser will give up current movement, set up next destination
		if(PlayerMovingDisplacement > AttackRange + DestinationChangingDistance)
		{
			FlyTimeline.Stop();
			OnFinishFlyingTask.Execute(BTComponent,true, false);
		}
	}
	
	
	
	// Object blocking checking
	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);
	
	FVector Start = GetActorLocation();
	FVector Offset = GetActorForwardVector() * 100;
	FVector End = Start + Offset; 
	
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, Start, End, 35.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	// if something on the way, stop timeline and execute delegate finish task function
	if(bHit)
	{
		FlyTimeline.Stop();
		OnFinishFlyingTask.Execute(BTComponent,true, true);
		return;
	}

	
	const FVector CurrentLocation = AIPostion;
	const FVector ToLocation = UKismetMathLibrary::VLerp(CurrentLocation,DestinationPos,Alpha);
	
	bool MovingSuccess = TeleportTo(ToLocation, GetActorRotation());
	// if(!MovingSuccess)
	// {
	// 	FlyTimeline.Stop();
	// 	OnFinishFlyingTask.Execute(true, true);
	// }
}

void AChaserEnemy::FinishFlying()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	
	IsCharacterMoving = false;
	OnFinishFlyingTask.Execute(BTComponent,true, false);
	World->GetTimerManager().ClearTimer(InterpTimerHandle);
}

void AChaserEnemy::TeleportUpdating(float Alpha)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	CharacterMesh->SetScalarParameterValueOnMaterials("Dissolve", Alpha);
}