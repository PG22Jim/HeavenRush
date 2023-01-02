// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetPlayerPos.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTService_SetPlayerPos : public UBTService_BlackboardBase
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToOverwrite)
	FBlackboardKeySelector BBKey_PlayerPos;

public:
	
};
