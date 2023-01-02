// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserAIController.h"

#include "ChaserEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


// void AChaserAIController::OnPossess(APawn* InPawn)
// {
// 	Super::OnPossess(InPawn);
//
// 	// start the behaviour tree
// 	if (BehaviorComponent != nullptr && BehaviorTree != nullptr)
// 	{
// 		if (BehaviorTree->BlackboardAsset != nullptr)
// 		{
// 			BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
// 		}
//
// 		BehaviorComponent->StartTree(*BehaviorTree);
// 	}
// }
//
//
// AChaserAIController::AChaserAIController(const FObjectInitializer& ObjectInitializer)
// {
// 	BehaviorComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComponent"));
// 	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
// }
void AChaserAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AChaserEnemy* OwnerClass = Cast<AChaserEnemy>(InPawn);
	if(OwnerClass != nullptr)
		OwnerClass->InitializeBehaviourTreeReference(BehaviorComponent);
	
}
