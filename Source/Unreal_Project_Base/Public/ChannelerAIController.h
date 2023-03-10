// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ChannelerAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API AChannelerAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()
	// COMPONENTS
	

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

	//AChannelerAIController(const FObjectInitializer& ObjectInitializer);

};

// inline void AChannelerAIController::OnPossess(APawn* InPawn)
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
// inline AChannelerAIController::AChannelerAIController(const FObjectInitializer& ObjectInitializer)
// {
// 	BehaviorComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComponent"));
// 	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
// }
