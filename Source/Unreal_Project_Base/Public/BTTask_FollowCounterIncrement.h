// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ChaserEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FollowCounterIncrement.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_FollowCounterIncrement : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesFromBlackBoardKey)
	FBlackboardKeySelector BBKey_FollowingCounter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	float CounterIncrement = 0.2f;
	
private:
	
};

inline EBTNodeResult::Type UBTTask_FollowCounterIncrement::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	float CurrentFollowCounter = BlackBoard->GetValueAsFloat(BBKey_FollowingCounter.SelectedKeyName);

	ACharacter* Character = OwnerComp.GetAIOwner()->GetCharacter();
	const AChaserEnemy* Chaser = Cast<AChaserEnemy>(Character);
	if(Chaser == nullptr) return EBTNodeResult::Failed;
	
	const bool bTimeSlowed = Chaser->GetIsTimeSlowed();

	if(bTimeSlowed)
		CurrentFollowCounter += CounterIncrement / 10;
	else
		CurrentFollowCounter += CounterIncrement;

	BlackBoard->SetValueAsFloat(BBKey_FollowingCounter.SelectedKeyName,CurrentFollowCounter);

	return EBTNodeResult::Succeeded;
}
