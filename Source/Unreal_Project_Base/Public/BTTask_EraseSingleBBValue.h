// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_EraseSingleBBValue.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_EraseSingleBBValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToErase)
	FBlackboardKeySelector BBKey_EQSMoveToPos;
	
private:
	
};

inline EBTNodeResult::Type UBTTask_EraseSingleBBValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	BlackBoard->ClearValue(BBKey_EQSMoveToPos.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}

