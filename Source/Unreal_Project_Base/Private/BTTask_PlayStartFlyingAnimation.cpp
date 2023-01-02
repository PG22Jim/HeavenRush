// Copyright © 2022 PotatoFive, All Rights Reserved


#include "BTTask_PlayStartFlyingAnimation.h"

#include "AIController.h"
#include "ChaserEnemy.h"

EBTNodeResult::Type UBTTask_PlayStartFlyingAnimation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;
	
	const UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return EBTNodeResult::Failed;

	const ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return  EBTNodeResult::Failed;
	
	// Getting a normal pawn reference to avoid const return for later casting
	APawn* AIPawn = AiController->GetPawn();
	if(AIPawn == nullptr) return EBTNodeResult::Failed;

	AChaserEnemy* ChaserClass = Cast<AChaserEnemy>(AIPawn);
	if(ChaserClass == nullptr) return EBTNodeResult::Failed;

	// // 
	// if(ChaserClass->GetIsByPassingObject())
	// {
	// 	return EBTNodeResult::Succeeded;
	// }

	ChaserClass->OnFinishBeginAnimationTask.BindDynamic(this, &UBTTask_PlayStartFlyingAnimation::StartFlyAnimFinish);

	ChaserClass->PlayBeginFlyingAnimation();

	return EBTNodeResult::InProgress;
}

void UBTTask_PlayStartFlyingAnimation::StartFlyAnimFinish(UBehaviorTreeComponent* OwnerComp)
{
	FinishLatentTask(*OwnerComp,EBTNodeResult::Succeeded);
}