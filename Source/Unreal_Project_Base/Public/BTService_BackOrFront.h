// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChaserEnemy.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_BackOrFront.generated.h"

/**
 * 
 */




UCLASS()
class UNREAL_PROJECT_BASE_API UBTService_BackOrFront : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesFromBlackBoardKey)
	FBlackboardKeySelector BBKey_ChaserCondition;

	uint8 ChaserConditionKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesFromBlackBoardKey)
	FBlackboardKeySelector BBKey_FollowingCounter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesFromBlackBoardKey)
	FBlackboardKeySelector BBKey_IsFollowingAtFront;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	float FollowingLimitation = 5.0f;
	
private:
};
