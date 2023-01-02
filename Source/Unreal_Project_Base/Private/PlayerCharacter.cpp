// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// const UWorld* World = GetWorld();
	// if(World == nullptr) return;
	//
	// World->GetTimerManager().SetTimer(MovementRecordTimerHandle,this, &APlayerCharacter::FindPlayerMovingDirection, RecordMoveDirInterval, true, -1);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	// Bind Player Attack events
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::StartHolding);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &APlayerCharacter::EndHolding);
	
	PlayerInputComponent->BindAction("FireBallAttack", IE_Pressed, this, &APlayerCharacter::FireBallAttack);
}

void APlayerCharacter::StartHolding()
{
	IsHolding = true;


	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	// Get time
	TotalAnimationTime = CalculateAttackTime();
	
	TryToNormalAttack();
	
	World->GetTimerManager().SetTimer(AttackChargeTimeHandler,this, &APlayerCharacter::TryToNormalAttack, TotalAnimationTime, true, -1);
	

}

void APlayerCharacter::TryToNormalAttack()
{
	if(!IsHolding || IsNormalAttacking) return;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	IsNormalAttacking = true;
	
	World->GetTimerManager().SetTimer(AttackRestrictTimerHandle,this, &APlayerCharacter::HandMovingBack, TotalAnimationTime - 0.1, false, -1);
	
	//IsAttacking = true;
	
	//World->GetTimerManager().SetTimer(HandAnimationResetTimeHandler,this, &APlayerCharacter::HandMovingBack, TimeToGetReady, false, -1);
	
	NormalAttack();
}

void APlayerCharacter::HandMovingBack()
{
	IsNormalAttacking = false;
}

void APlayerCharacter::SetTimerToAttack()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	// Get time
	float TotalTime = CalculateAttackTime();
	
	//World->GetTimerManager().SetTimer(AttackChargeTimeHandler,this, &APlayerCharacter::AnimationStart, TotalTime, true, -1);
}

void APlayerCharacter::EndHolding()
{
	IsHolding = false;

	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	World->GetTimerManager().ClearTimer(AttackChargeTimeHandler);
}

void APlayerCharacter::FindPlayerMovingDirection()
{
	AllocatePos();

	// Early return if there is no current or previous position
	if(PlayerCurrentPos == FVector{0,0,0} || PlayerPreviousPos == FVector{0,0,0}) return;


	const FVector CalculatedDirection = FindMovingDirection();

	if(CalculatedDirection != FVector{0,0,0})
	{
		MovingDirection = CalculatedDirection;
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,FString::Printf(TEXT("%s"), *MovingDirection.ToString()));
	}
}

void APlayerCharacter::StartCharging()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	World->GetTimerManager().SetTimer(FireballChargeTimerHandle,this, &APlayerCharacter::ChargingFireBall, 0.1, true, 0);
}

void APlayerCharacter::ChargingFireBall()
{
	CurrentFireballCharge += FireballChargeIncrement;
	//CurrentFireballCharge = UKismetMathLibrary::Clamp(CurrentFireballCharge,0,MaxFireballCharge);
}

void APlayerCharacter::EndChargingFireBall()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	World->GetTimerManager().ClearTimer(FireballChargeTimerHandle);
	
	FireBallAttack();
}

float APlayerCharacter::CalculateAttackTime()
{
	if(IsOverdriveState)
		return NormalAttackAnimationTime / 2;
	
	return NormalAttackAnimationTime;
}

void APlayerCharacter::AllocatePos()
{
	FVector CurrentPos_WithoutZ = GetActorLocation();
	CurrentPos_WithoutZ.Z = 0; 

	// First time recording 
	if(PlayerCurrentPos == FVector{0,0,0})
	{
		PlayerCurrentPos = CurrentPos_WithoutZ;
		return;
	}
	
	// Player need to move to certain distance in order to record new position

	const float DistanceToPrevious = UKismetMathLibrary::Vector_Distance(PlayerCurrentPos, CurrentPos_WithoutZ);

	if(DistanceToPrevious < DistanceRequireToRecord) return;
	
	PlayerPreviousPos = PlayerCurrentPos;
	PlayerCurrentPos = CurrentPos_WithoutZ;
}

FVector APlayerCharacter::FindMovingDirection()
{
	const FVector Offset = PlayerCurrentPos - PlayerPreviousPos;
	const FVector Direction = UKismetMathLibrary::Normal(Offset);

	return Direction;
}


