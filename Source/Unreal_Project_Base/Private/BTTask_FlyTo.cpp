// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FlyTo.h"

#include "AIController.h"
#include "ChaserEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

EBTNodeResult::Type UBTTask_FlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	// Grab necessary references 
	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;
	
	const UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return EBTNodeResult::Failed;

	const ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return  EBTNodeResult::Failed;

	// Getting a normal pawn reference to avoid const return for later casting
	APawn* AIPawn = AiController->GetPawn();
	if(AIPawn == nullptr) return EBTNodeResult::Failed;

	AChaserEnemy* ChaserClass = Cast<AChaserEnemy>(AIPawn);
	if(ChaserClass == nullptr) return EBTNodeResult::Failed;

	// if(ChaserClass->GetBehaviourTreeReference() == nullptr)
	// {
	// 	ChaserClass->InitializeBehaviourTreeReference(&OwnerComp);
	// }
	
	ChaserClass->OnFinishFlyingTask.BindDynamic(this, &UBTTask_FlyTo::FinishChaserFlying);
	
	const FVector MoveToPos = BlackBoard->GetValueAsVector(BBKey_MoveToPos.SelectedKeyName);
	
	// call try fly function from AChaserEnemy class 
	// ChaserClass->SimpleFly(MoveToPos, IsBypassing);
	ChaserClass->StartComplexFlying(MoveToPos, IsBypassing);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_FlyTo::FinishChaserFlying(UBehaviorTreeComponent* OwnerComp, bool bIsSuccess, bool bDoesGetBlocked)
{
	UBlackboardComponent* BlackBoard = OwnerComp->GetBlackboardComponent();
	if(BlackBoard == nullptr) return;
	
	BlackBoard->SetValueAsBool("DoesGetStoped", bDoesGetBlocked);
	
	if(bIsSuccess)
	{
		FinishLatentTask(*OwnerComp,EBTNodeResult::Succeeded);
		return;
	}
	FinishLatentTask(*OwnerComp,EBTNodeResult::Failed);
}