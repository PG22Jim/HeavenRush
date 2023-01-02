// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PosOffsetAdjustment.h"

#include "ChaserAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type UBTTask_PosOffsetAdjustment::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	// Calculate final pos adjusted by offset
	const FVector OriginalPos = BlackBoard->GetValueAsVector(BBKey_DestinationPos.SelectedKeyName);

	const float OffsetX = UKismetMathLibrary::RandomFloatInRange(OffsetValue * -1, OffsetValue);
	const float OffsetY = UKismetMathLibrary::RandomFloatInRange(OffsetValue * -1, OffsetValue);
	const float OffsetZ = UKismetMathLibrary::RandomFloatInRange(OffsetValue * -1, OffsetValue);

	const FVector OffsetPos = {OffsetX,OffsetY,OffsetZ};

	FVector FinalPos = OriginalPos + OffsetPos;
	
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	const TArray<AActor*> IgnoreActors;
	
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, OriginalPos, FinalPos,UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);
	
	if(bHit) FinalPos = Hit.Location;
	
	BlackBoard->SetValueAsVector(BBKey_AdjustedPos.SelectedKeyName,FinalPos);
	
	return EBTNodeResult::Succeeded;
	
}
