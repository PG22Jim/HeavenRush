// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetFinalMoveToPos.h"

#include "ChaserAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type UBTTask_SetFinalMoveToPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;
	
	
	// Calculate final pos adjusted by offset
	
	const AChaserAIController* AIController = Cast<AChaserAIController>(OwnerComp.GetAIOwner());
	if(AIController == nullptr) return EBTNodeResult::Failed;

	const ACharacter* AiCharacter = AIController->GetCharacter();
	if(AiCharacter == nullptr) return EBTNodeResult::Failed;
	
	const UCapsuleComponent* AICapsule = AiCharacter->GetCapsuleComponent();
	if(AICapsule == nullptr) return EBTNodeResult::Failed;

	const float AIHalfHeight = AICapsule->GetUnscaledCapsuleHalfHeight();

	const FVector PlayerPos = BlackBoard->GetValueAsVector(BBKey_PlayerPos.SelectedKeyName);

	FVector EQSPos = BlackBoard->GetValueAsVector(BBKey_DestinationPos.SelectedKeyName);

	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, later add player class to be ignored
	TArray<AActor*> IgnoreActors;
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);
	IgnoreActors.Add(PlayerCharacter);

	// Line trace to see if 
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, PlayerPos, EQSPos,UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);
	
	if(bHit) EQSPos = Hit.Location;
	
	EQSPos.Y = PlayerPos.Y + AIHalfHeight;

	BlackBoard->SetValueAsVector(BBKey_DestinationPos.SelectedKeyName,EQSPos);
	
	return EBTNodeResult::Succeeded;
}
