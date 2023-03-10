// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RotateToNextDegree.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_RotateToNextDegree : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToGrab)
	FBlackboardKeySelector BBKey_NextRotationDegree;
	
};
