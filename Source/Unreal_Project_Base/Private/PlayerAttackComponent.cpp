// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackComponent.h"

#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UPlayerAttackComponent::UPlayerAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerAttackComponent::Initialization(ACharacter* Player, UCameraComponent* Camera)
{
	PLayerCamera = Camera;
	PlayerCharacter = Player;
	PlayerMovement = Player->GetCharacterMovement();
}

bool UPlayerAttackComponent::FindEnemyToMelee()
{
	FoundEnemies = EnemyInRange();
	if(FoundEnemies.Num() == 0) return false;

	RemoveEnemyCannotMoveTo(FoundEnemies);
	if(FoundEnemies.Num() == 0) return false;
	if(FoundEnemies.Num() == 1)
	{
		ABaseEnemy* CastingEnemy = Cast<ABaseEnemy>(FoundEnemies[0]);
		if(CastingEnemy == nullptr) return false;
		CloseEnemy = CastingEnemy;
		return true;
	}
	ABaseEnemy* FirstEnemy = Cast<ABaseEnemy>(FoundEnemies[0]);
	if(FirstEnemy == nullptr) return false;
	CloseEnemy = FirstEnemy;

	for (int32 i = 1; i < FoundEnemies.Num(); i++)
	{
		ABaseEnemy* IterationEnemy = Cast<ABaseEnemy>(FoundEnemies[i]);
		if(IterationEnemy == nullptr) continue;

		const float IterateDistance = GetDistanceToPlayer(IterationEnemy);
		const float ClosestDistance = GetDistanceToPlayer(CloseEnemy);
		
		if(IterateDistance < ClosestDistance) CloseEnemy = IterationEnemy;
	}
	return true;
}

TArray<AActor*> UPlayerAttackComponent::EnemyInRange()
{
	TArray<AActor*> FoundActorList;
	const UWorld* World = GetWorld();
	if(World == nullptr) return FoundActorList;

	const FVector CameraLocation = PLayerCamera->GetComponentLocation();
	const FVector XVector = UKismetMathLibrary::Conv_RotatorToVector(PlayerCharacter->GetBaseAimRotation());
	const float HalfDistance = OffSetDistance / 2;
	const FVector BoxPos = CameraLocation + (XVector * HalfDistance);

	const TArray<AActor*> IgnoreActors;
	const FVector BoxExtent = UKismetMathLibrary::MakeVector(HalfDistance, DetectionWidthHeight, DetectionWidthHeight);
	
	UKismetSystemLibrary::BoxOverlapActors(World,BoxPos, BoxExtent, FilterType, FilteringClass, IgnoreActors,FoundActorList);
	return FoundActorList;
}

void UPlayerAttackComponent::RemoveEnemyCannotMoveTo(TArray<AActor*> EnemiesInRange)
{
	const UWorld* World = GetWorld();
	if(PlayerCharacter == nullptr || World == nullptr) return;

	TArray<AActor*> EnemiesCanMoveTo;
	for (AActor* EachEnemy : EnemiesInRange)
	{
		const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		const FVector EnemyLocation = EachEnemy->GetActorLocation();
		const float PlayerHalfHeight = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		const float Radius = PlayerHalfHeight / 4;

		// Hit result
		FHitResult Hit;
		// Empty array of ignoring actor, maybe add Enemies classes to be ignored
		TArray<AActor*> IgnoreActors;

		IgnoreActors = EnemiesInRange;

		IgnoreActors.Add(PlayerCharacter);

		// Line trace by channel from Kismet System Library 
		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, PlayerLocation, EnemyLocation, Radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Hit,true);
		
		if(!bHit) EnemiesCanMoveTo.Add(EachEnemy);
	}
	FoundEnemies = EnemiesCanMoveTo;
}


float UPlayerAttackComponent::GetDistanceToPlayer(ABaseEnemy* EnemyRef)
{
	const FVector EnemyPos = EnemyRef->GetActorLocation();
	const FVector PlayerPos = PlayerCharacter->GetActorLocation();
	const float Displacement = UKismetMathLibrary::Vector_Distance(EnemyPos, PlayerPos); 
	return UKismetMathLibrary::Abs(Displacement);
}

void UPlayerAttackComponent::TryTeleport()
{
	if(IsSnaping) return;
	
	if(!FindEnemyToMelee())
	{
		// if there is no enemy found, do only animation;
		// TODO: play animation
		
		return;
	}

	// Reference check
	if(PlayerCharacter == nullptr) return;
	if(PlayerMovement == nullptr) return;

	// Reset velocity
	PlayerMovement->Velocity = {0,0,0};

	

	SnapToDestination = FindSnapDestinationPos(CloseEnemy);
	if(SnapToDestination == FVector{0,0,0})
	{
		// TODO: Play Animation and return

		return;
	}

	IsSnaping = true;
	//SnapMovementTimeline.PlayFromStart();
	// TODO: play animation
	
}

FVector UPlayerAttackComponent::FindSnapDestinationPos(ABaseEnemy* SnapToEnemy)
{
	const UWorld* World = GetWorld();
	if(PlayerCharacter == nullptr || PlayerMovement == nullptr || World == nullptr) return FVector{0,0,0};

	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector EnemyLocation = SnapToEnemy->GetActorLocation();
	const float EnemyCapsuleRadius = SnapToEnemy->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const float PlayerCapsuleRadius = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const float CapsuleQuarterHeight = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2;

	// Line trace by channel from Kismet System Library 
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, PlayerLocation, EnemyLocation, CapsuleQuarterHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Hit,true);
		
	if(bHit)
	{
		return Hit.Location;
	}

	const FVector Direction = UKismetMathLibrary::Normal((EnemyLocation - PlayerLocation));
	
	const FVector MinusVector = Direction * EnemyCapsuleRadius * 4; 
	const FVector TraceEnd = Hit.TraceEnd - MinusVector;
	
	
	return TraceEnd;

	
	// const float EnemyCapsuleRadius = SnapToEnemy->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// const float PlayerCapsuleRadius = PlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	//
	// const FVector EnemyLocation = SnapToEnemy->GetActorLocation();
	// const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	// const FVector Direction = UKismetMathLibrary::Normal((EnemyLocation - PlayerLocation));
	//
	// const FVector MinusVector = Direction * ((EnemyCapsuleRadius * 2.5) + PlayerCapsuleRadius); 
	// const FVector TeleportToPos = EnemyLocation - MinusVector;
	//
	// const float TotalDistance = UKismetMathLibrary::Abs(UKismetMathLibrary::Vector_Distance(PlayerLocation,TeleportToPos));
	//
	// return PlayerLocation + (Direction * TotalDistance);
}


// Called when the game starts
void UPlayerAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// // Timeline initialization
	// FOnTimelineFloat SnapProgressUpdate;
	// SnapProgressUpdate.BindDynamic(this, &UPlayerAttackComponent::SnapMovementUpdate);
	//
	// FOnTimelineEvent SnapProgressFinish;
	// SnapProgressFinish.BindDynamic(this, &UPlayerAttackComponent::SnapMovementFinish);
	//
	// SnapMovementTimeline.AddInterpFloat(SnapMovementCurveFloat, SnapProgressUpdate);
	// SnapMovementTimeline.SetTimelineFinishedFunc(SnapProgressFinish);
}


// Called every frame
void UPlayerAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//SnapMovementTimeline.TickTimeline(DeltaTime);
	// ...
}

// void UPlayerAttackComponent::SnapMovementUpdate(float Alpha)
// {
// 	if(PlayerCharacter == nullptr) return;
// 	
// 	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
//
// 	const FVector Destination = UKismetMathLibrary::VLerp(PlayerLocation, SnapToDestination, Alpha);
//
// 	const FRotator DestRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, Destination);
// 	
// 	const bool bTeleport = PlayerCharacter->TeleportTo(Destination, DestRotation);
// }
//
// void UPlayerAttackComponent::SnapMovementFinish()
// {
// 	if(PlayerCharacter == nullptr || PlayerMovement == nullptr) return;
// 	
// 	const FVector ForwardVec = PlayerCharacter->GetActorForwardVector();
//
// 	PlayerMovement->Velocity = ForwardVec * SimulateVelocityValue;
//
// 	IsSnaping = false;
// }


