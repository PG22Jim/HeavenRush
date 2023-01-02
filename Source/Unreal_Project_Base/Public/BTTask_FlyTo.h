// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FlyTo.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_FlyTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UFUNCTION()
	void FinishChaserFlying(UBehaviorTreeComponent* OwnerComp, bool bIsSuccess, bool DoesGetStoped);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToGrab)
	FBlackboardKeySelector BBKey_PlayerPos;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToGrab)
	FBlackboardKeySelector BBKey_MoveToPos;

	// Higher will be slower speed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	float InterSpeedDivider = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	bool IsBypassing = false;
	
private:

	UPROPERTY()
	UBehaviorTreeComponent* SelfOwner;
	
};
