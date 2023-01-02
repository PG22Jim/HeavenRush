// Copyright © 2022 PotatoFive, All Rights Reserved


#include "BTTask_StopFlying.h"

#include "AIController.h"
#include "ChaserEnemy.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_StopFlying::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController == nullptr) return EBTNodeResult::Failed;
	
	ACharacter* AICharacter = AIController->GetCharacter();
	if(AICharacter == nullptr) return EBTNodeResult::Failed;
	
	AChaserEnemy* ChaserClass = Cast<AChaserEnemy>(AICharacter);
	if(ChaserClass == nullptr) return EBTNodeResult::Failed;

	ChaserClass->GetFlyTimeline()->Stop();

	if(ChaserClass->GetIsCharacterMoving())
		ChaserClass->SetIsCharacterMoving(false);


	
	return EBTNodeResult::Succeeded;
}