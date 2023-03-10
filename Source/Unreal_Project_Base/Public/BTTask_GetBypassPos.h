// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ChaserEnemy.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetBypassPos.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FSideAngleInfo
{
	GENERATED_BODY()

public:

	FSideAngleInfo()
	{
		SideAlpha = 2;
		SideDestination = {0,0,0};
		IsVerticalDestination = false;
	}
	
	FSideAngleInfo(float Alpha, FVector Destination, bool VerticalDestination)
	{
		SideAlpha = Alpha;
		SideDestination = Destination;
		IsVerticalDestination = VerticalDestination;
	}

	float GetSideAlpha() const { return SideAlpha; }
	FVector GetSideDestination() const { return SideDestination; }
	bool GetIsVerticalDestination() const { return IsVerticalDestination; }

private:
	UPROPERTY()
	float SideAlpha;

	UPROPERTY()
	FVector SideDestination;

	UPROPERTY()
	bool IsVerticalDestination;
};

UCLASS()
class UNREAL_PROJECT_BASE_API UBTTask_GetBypassPos : public UBTTask_BlackboardBase
{

	
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FVector PlaceToMove(const int32 InvalidNum, const ACharacter* AICharacter, UBlackboardComponent* BBComp);

	FVector GetPositionToMove(ACharacter* ChaserRef);

	

	FSideAngleInfo SideAngleAlpha(bool bIsLeft, FVector ChaserPosition, FRotator FacingRotation, FVector RightVector, float CurrentAlpha);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValueToWrite)
	FBlackboardKeySelector MoveToPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValueToWrite)
	FBlackboardKeySelector InValidDirectionToBypass;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Property)
	float ByPassDisplacement = 300.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Property)
	float ByPassVerticalOffset = 100.0f;
	
	GENERATED_BODY()
	
};
