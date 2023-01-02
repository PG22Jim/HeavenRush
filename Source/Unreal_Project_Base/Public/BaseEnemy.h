// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BaseEnemy.generated.h"



DECLARE_DYNAMIC_DELEGATE_TwoParams(FFinishRotationTask, bool, bIsSuccess, bool, DoesGetStoped);


UCLASS()
class UNREAL_PROJECT_BASE_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()
	
	
public:
	// Sets default values for this character's properties
	ABaseEnemy();

	void StopRotation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Property)
	float Health = 100;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Property)
	bool IsCanBeAttacked = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=AIMovement)
	bool IsCharacterMoving = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Rotation)
	float CurrentRotateDegree = 0;

	float OverwritingDegree;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Rotation)
	float MaxRotateDegree = 45;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Rotation)
	float MinRotateDegree = -45;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Rotation)
	float RotatingDeltaTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Rotation)
	UCurveFloat* RotateTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= OnDeath)
	UCurveFloat* DissolveCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= EnteringMap)
	UCurveFloat* EnteringCurveFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Property)
	bool CanBeDamaged = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Materials)
	int32 GlowingMaterialIndex = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Materials)
	UMaterialInstanceDynamic* GlowingMaterialInstanceREF;
	
	// Timer Handle for calling parkour functions in time
	FTimerHandle RotateTimerHandle;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TryRotateTo(float Current, float Next);
	
	float GetCurrentDegree() const { return CurrentRotateDegree;}
	void SetNextDegree(const float NextDegree) { OverwritingDegree = NextDegree;}
	float GetMaxDegree() const { return MaxRotateDegree;}
	float GetMinDegree() const { return MinRotateDegree;}

	bool IsEnemyRotating() const {return IsRotating;}
	
	UFUNCTION()
	void RotateUpdate(float Alpha);

	UFUNCTION()
	void RotateFinish();

	UFUNCTION(BlueprintCallable)
	void OnDissolving();

	UFUNCTION()
	void OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DisableCollision();

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamageEvent(float DecreasedDamage);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void EnableCollision();
	
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetHitMarker();

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerDamageEffect(const UDamageType* DamageType);
	
	UFUNCTION()
	void DissolveUpdate(float Alpha);

	UFUNCTION()
	void DissolveFinish();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SpawnMagicCircle();

	UFUNCTION(BlueprintCallable)
	void SetEmissiveColor(FVector NewColor);

	UFUNCTION(BlueprintCallable)
	void ReSetEmissiveColor();
	
	UFUNCTION()
	void EnteringUpdate(float Alpha);

	UFUNCTION()
	void EnteringFinish();

	UFUNCTION(BlueprintCallable)
	void EnablePhysics();

	UFUNCTION(BlueprintCallable)
	void SetGlowingMaterialInst();

	UFUNCTION(BlueprintCallable)
	void SetIsNotAttacking();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SlowlyTurnTo();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsKillHit(float Damage);

	bool GetIsCharacterMoving() {return IsCharacterMoving;}

	void SetIsCharacterMoving(bool IsMoving) {IsCharacterMoving = IsMoving;}

	void StopBehaviourTree();

	void ContinueBehaviourTree();

	// Delegate Reference to be used
	FFinishRotationTask OnFinishRotationTask;
	
private:
	// Timelines
	FTimeline RotateTimeline;
	FTimeline DeathTimeline;
	FTimeline ResetTimeline;
	FTimeline EnteringTimeline;

	FRotator NextRotation;

	FTimerHandle ResetPhysicsTimer;

	bool IsRotating = false;
	
	bool IsDead = false;

	void StartPlayDissolving();

	void DisablePhysics();

	virtual void StopChaserMovement();
	
	void ResetRotation();
};
