// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_BackOrFront.h"

#include "Enum_ChaserCondition.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_BackOrFront::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	const bool bFollowAtFront = BlackBoard->GetValueAsBool(BBKey_IsFollowingAtFront.SelectedKeyName);

	if(bFollowAtFront) return;
	
	const float CurrentFollowingCounter = BlackBoard->GetValueAsFloat(BBKey_FollowingCounter.SelectedKeyName);

	// Later Uncomment it
	// if(CurrentFollowingCounter >= FollowingLimitation)
	// {
	// 	BlackBoard->SetValueAsBool(BBKey_IsFollowingAtFront.SelectedKeyName, true);
	// 	BlackBoard->SetValueAsFloat(BBKey_FollowingCounter.SelectedKeyName,0);
	// }
	
	
	//const uint8 ConditionNum = BlackBoard->GetValueAsEnum(ChaserCondition.SelectedKeyName);

	//Chaser_AIState BackCondition = Chaser_AIState::Back;

	
	
	// if(ConditionNum == 1)
	
}
