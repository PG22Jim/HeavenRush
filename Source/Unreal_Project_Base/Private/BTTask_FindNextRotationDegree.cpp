// Copyright © 2022 PotatoFive, All Rights Reserved


#include "BTTask_FindNextRotationDegree.h"
#include "AIController.h"
#include "BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

EBTNodeResult::Type UBTTask_FindNextRotationDegree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	const AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return EBTNodeResult::Failed;

	ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return  EBTNodeResult::Failed;
	
	ABaseEnemy* EnemyClass = Cast<ABaseEnemy>(AiCharacter);

	if(EnemyClass->IsEnemyRotating())
	{
		return  EBTNodeResult::Failed;
	}

	
	const float CurrentRotatingDegree = EnemyClass->GetCurrentDegree();
	const float MaxRotatingDegree = EnemyClass->GetMaxDegree();
	const float MinRotatingDegree = EnemyClass->GetMinDegree();

	const float NextDegree = GetNextDegree(CurrentRotatingDegree, MaxRotatingDegree, MinRotatingDegree);
	EnemyClass->SetNextDegree(NextDegree);
	
	BlackBoard->SetValueAsFloat(BBKey_NextRotationDegree.SelectedKeyName,NextDegree);
	return EBTNodeResult::Succeeded;
}

float UBTTask_FindNextRotationDegree::GetNextDegree(float Current, float Max, float Min)
{
	if(Current == Min)
		Current = Max;
	else if(Current == Max)
		Current = Min;
	else if(Current == 0)
	{
		const float RandomFloat = UKismetMathLibrary::RandomFloatInRange(0,100);
		if(RandomFloat >= 50)
			Current = Max;
		else
			Current = Min;
	}
	return Current;
}