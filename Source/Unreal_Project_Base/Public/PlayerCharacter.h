// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class UNREAL_PROJECT_BASE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Variables
	FTimerHandle AttackChargeTimeHandler;
	FTimerHandle HandAnimationResetTimeHandler;
	FTimerHandle AttackRestrictTimerHandle;
	FTimerHandle AttackContinueTimerHandle;
	FTimerHandle FireballChargeTimerHandle;
	FTimerHandle MovementRecordTimerHandle;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=SpeedGauge)
	bool IsOverdriveState = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	FKey NormalAttackKey;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	bool IsHolding = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	float TotalAnimationTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	bool IsNormalAttacking = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	bool IsAttacking = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	bool CanNormalAttack = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	float NormalAttackAnimationTime = 2.0f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	float ChargeTime;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	float CheckHoldingTime;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	float ChargeAttackValue = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Attack)
	float MaxCharge = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=FireBall)
	float MaxFireballCharge = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=FireBall)
	float CurrentFireballCharge = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=FireBall)
	float FireballChargeIncrement = 0.05;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=MovementDirection)
	FVector MovingDirection = {0,0,0};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=MovementDirection)
	float RecordMoveDirInterval = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=MovementDirection)
	float DistanceRequireToRecord = 30;
	
	FVector PlayerCurrentPos = {0,0,0};

	FVector PlayerPreviousPos = {0,0,0};
	
	
	// Functions
	UFUNCTION(BlueprintImplementableEvent)
	void FireBallAttack();
	
	UFUNCTION(BlueprintImplementableEvent)
	void NormalAttack();

	UFUNCTION(BlueprintImplementableEvent)
	void ChargeAttack();

	UFUNCTION(BlueprintImplementableEvent)
	void ResetAttack();

	UFUNCTION(BlueprintCallable)
	void EndHolding();

	UFUNCTION(BlueprintCallable)
	void FindPlayerMovingDirection();


	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsOverdrived() const {return IsOverdriveState;}

	FVector GetMovingDirection() const {return MovingDirection;}

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartWallRunSound(bool bLeftWallRun);
	
private:
	
	void StartHolding();
	void TryToNormalAttack();
	void HandMovingBack();
	void SetTimerToAttack();
	void Holding();

	void StartCharging();
	void ChargingFireBall();
	void EndChargingFireBall();


	float CalculateAttackTime();



	void AllocatePos();
	FVector FindMovingDirection();
	
};
