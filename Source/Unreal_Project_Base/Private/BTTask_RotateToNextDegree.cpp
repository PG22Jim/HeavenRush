// Copyright © 2022 PotatoFive, All Rights Reserved


#include "BTTask_RotateToNextDegree.h"
#include "AIController.h"
#include "BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_RotateToNextDegree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return EBTNodeResult::Failed;

	ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return  EBTNodeResult::Failed;

	ABaseEnemy* EnemyClass = Cast<ABaseEnemy>(AiCharacter);
	
	const float CurrentRotationAngle = EnemyClass->GetCurrentDegree(); 

	const float NextRotationAngle = BlackBoard->GetValueAsFloat(BBKey_NextRotationDegree.SelectedKeyName);
	
	EnemyClass->TryRotateTo(CurrentRotationAngle, NextRotationAngle);

	return EBTNodeResult::Succeeded;
}