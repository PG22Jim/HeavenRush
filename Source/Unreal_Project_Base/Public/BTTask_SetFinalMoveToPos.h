// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetFinalMoveToPos.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_SetFinalMoveToPos : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToOverwrite)
	FBlackboardKeySelector BBKey_DestinationPos;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToGrab)
	FBlackboardKeySelector BBKey_PlayerPos;
	
};
