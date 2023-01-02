// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "ChaserAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/TimelineComponent.h"
#include "ChaserEnemy.generated.h"

/**
 * 
 */
class UTimelineComponent;

UENUM(BlueprintType)
enum Enum_ChaserCondition
{
	Back UMETA(DisplayName = "BACK"),
	Front UMETA(DisplayName = "FRONT"),
	None 
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FFinishFlyingTask, UBehaviorTreeComponent*, BehaviorTreeReference, bool, bIsSuccess, bool, DoesGetStoped);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFinishBeginAnimationTask, UBehaviorTreeComponent*, BehaviorTreeReference);

UCLASS()
class UNREAL_PROJECT_BASE_API AChaserEnemy : public ABaseEnemy
{
	GENERATED_BODY()
public:

	AChaserEnemy();

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UTimelineComponent* TimelineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ThreatSystem)
	int32 ThreatLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= ThreatSystem)
	bool IsAssignedToAttack = false;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=TimeSlowSetting)
	bool IsTimeSlowed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AI)
	UBehaviorTreeComponent* BTComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AI)
	UBlackboardComponent* BBComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AI)
	AChaserAIController* CurrentChaserController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=AiMovement)
	float FollowingSpeed = 300;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=AiMovement)
	float CloseMovementSpeed = 100;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying)
	float FlySpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Flying)
	float BypassSpeedMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Flying)
	float DestinationChangingDistance = 300.0f;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying)
	UCurveFloat* FlyingTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Teleportation)
	UCurveFloat* TeleportTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Property)
	float AttackRange = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Property)
	float AttackWidth = 300;

	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
private:
	FTimerHandle InterpTimerHandle;
	FTimerHandle TeleportTimerHandle;

	FTimerHandle ComplexFlyTimerHandle;


	// Timelines
	FTimeline FlySlowTimeline;

	FVector AIPostion;
	FVector PlayerLocation;
	FVector DestinationPos;
	FVector TeleportPos = {0,0,0};
	float DeltaTime;
	float VInterpSpeed;
	float TravelDistancePerUpdate = 20;
	float NormalSpeed = 20;
	
	bool IsChaserFlying = false;
	bool IsByPassingObject = false;
	bool IsByPassingDoneOnce = false;
	bool IsVerticalByPass = false;

	float GetAdjustedFlySpeed();


	void InintializeBTAndBBComponent();

	void ComplexFlying();

	FVector UpdateNextPos(FVector DestionationLocation, float CurrentSpeed);

	float UpdateSpeed();

public:

	FFinishFlyingTask OnFinishFlyingTask;

	FFinishBeginAnimationTask OnFinishBeginAnimationTask;

	// Timelines
	FTimeline FlyTimeline;

	FTimeline TeleportTimeline;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayBeginFlyingAnimation();

	UFUNCTION(BlueprintCallable)
	void FinishBeginFlyingAnimation();
	
	
	float GetFollowSpeed() const {return FollowingSpeed;}
	
	float GetCloseMoveSpeed() const {return CloseMovementSpeed;}

	float GetAttackRange() const {return AttackRange;}
	float GetAttackWidth() const {return AttackWidth;}
	
	UBehaviorTreeComponent* GetBTComponent() const { return BTComponent; }

	UBlackboardComponent* GetBBComponent() const { return BBComponent; }
	
	void SimpleFly(FVector MoveToPos, bool IsBypassing);

	void StartTeleportingTo(FVector TeleportingPos);

	bool GetIsChaserFlying() const {return IsChaserFlying;}

	bool GetIsTimeSlowed() const {return IsTimeSlowed;}

	bool GetIsByPassingObject() const {return IsByPassingObject;}
	
	bool GetIsVerticalByPass() const {return IsVerticalByPass;}
	
	void SetIsVerticalByPass(bool VerticalMoving) {IsVerticalByPass = VerticalMoving;}
	void SetIsByPassingDoneOnce(bool ByPassingDoneOnce) {IsByPassingDoneOnce = ByPassingDoneOnce;}

	FTimeline* GetFlyTimeline() {return &FlyTimeline;}

	virtual void StopChaserMovement() override;

	int32 GetThreatLevel() { return ThreatLevel;}

	void IncreaseThreatLevel() { ThreatLevel++;}

	void ResetThreatLevel() { ThreatLevel = 1;}

	bool GetIsAssignedToAttack() { return IsAssignedToAttack;}

	void SetIsAssignedToAttack(bool bAttack) { IsAssignedToAttack = bAttack;}

	void SetCanAttack();

	UBehaviorTreeComponent* GetBehaviourTreeReference() {return BTComponent;}
	
	void InitializeBehaviourTreeReference(UBehaviorTreeComponent* BT) { BTComponent = BT;}

	virtual void ReceiveDamageEvent(float DecreasedDamage) override;
	
	UFUNCTION()
	void FlyTo(float Alpha);

	UFUNCTION()
	void FinishFlying();

	UFUNCTION()
	void TeleportUpdating(float Alpha);

	UFUNCTION()
	void Teleportation();

	
	void StartComplexFlying(FVector MoveToPos , bool IsByPassing);
};