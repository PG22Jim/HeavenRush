// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "ChaserAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API AChaserAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

	virtual void OnPossess(APawn* InPawn) override;
	// // COMPONENTS
	//
	// UPROPERTY( EditInstanceOnly )
	// UBehaviorTreeComponent* BehaviorComponent;
	//
	// UPROPERTY( EditInstanceOnly )
	// UBlackboardComponent* BlackboardComponent;
	//
	// // PROPERTIES
	//
	//
	// UPROPERTY( EditDefaultsOnly, Category="AI" )
	// UBehaviorTree* BehaviorTree;
	//
	// virtual void OnPossess(APawn* InPawn) override;

public:
	
	// AChaserAIController(const FObjectInitializer& ObjectInitializer);

	
};
