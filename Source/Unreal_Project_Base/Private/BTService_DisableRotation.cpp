// Copyright © 2022 PotatoFive, All Rights Reserved


#include "BTService_DisableRotation.h"

#include "AIController.h"
#include "BaseEnemy.h"

void UBTService_DisableRotation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return;

	ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return;
	
	ABaseEnemy* EnemyClass = Cast<ABaseEnemy>(AiCharacter);
	if(EnemyClass == nullptr) return;
	
	if(EnemyClass->IsEnemyRotating())
	{
		EnemyClass->StopRotation();
	}
	
}