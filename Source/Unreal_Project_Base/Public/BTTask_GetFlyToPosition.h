// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetFlyToPosition.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_GetFlyToPosition : public UBTTask_BlackboardBase
{

	GENERATED_BODY()

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FVector GetSupposMoveToPosition(FVector PlayerPos, FVector AiPosition);
	
	FVector GetValidPosAroundPlayer(ACharacter* PlayerCharacter, FVector PlayerPos, FVector PositionToMove);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Property)
	float DistanceAroundPlayer = 300.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= ValueToWrite)
	FBlackboardKeySelector BBKey_MoveToPos;
	
	
};
