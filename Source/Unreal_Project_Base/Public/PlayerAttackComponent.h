// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerAttackComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREAL_PROJECT_BASE_API UPlayerAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAttackComponent();

	// Callable functions to be called from player blueprint
	UFUNCTION(BlueprintCallable)
	void Initialization(ACharacter* Player, UCameraComponent* Camera);

	UFUNCTION(BlueprintCallable)
	bool FindEnemyToMelee();

	UFUNCTION(BlueprintCallable)
	FVector FindSnapDestinationPos(ABaseEnemy* SnapToEnemy);
	
	UFUNCTION(BlueprintCallable)
	void TryTeleport();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Reference to player character and player movement, Add UPROPERTY to avoid getting garbage collected
	UPROPERTY()
	ACharacter* PlayerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* PlayerMovement;
	UPROPERTY()
	UCameraComponent* PLayerCamera;


	// UPROPERTY()
	// FTimeline SnapMovementTimeline;

	// UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	// UCurveFloat* SnapMovementCurveFloat;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	float OffSetDistance = 600;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	float DetectionWidthHeight = 150;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	float SimulateVelocityValue = 1000.0f;
	
	UPROPERTY()
	TArray<AActor*> FoundEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MeleeSnapSetting)
	FVector SnapToDestination = FVector{0,0,0};

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=MeleeSnapSetting)
	bool IsSnaping = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=MeleeSnapSetting)
	ABaseEnemy* CloseEnemy;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	TArray<AActor*> EnemyInRange();

	void RemoveEnemyCannotMoveTo(TArray<AActor*> EnemiesInRange);
	
	float GetDistanceToPlayer(ABaseEnemy* EnemyRef);
	
	// void SnapMovementUpdate(float Alpha);
	//
	// void SnapMovementFinish();
		
};