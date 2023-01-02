// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckDistance.h"

#include "ChaserAIController.h"
#include "ChaserEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UBTService_CheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	ACharacter* AICharacter = OwnerComp.GetAIOwner()->GetCharacter();
	const FVector AILocation = AICharacter->GetActorLocation();
	
	const FVector PlayerLocation = BlackBoard->GetValueAsVector(BBKey_PlayerPos.SelectedKeyName);

	const float Distance = UKismetMathLibrary::Abs(UKismetMathLibrary::Vector_Distance(AILocation,PlayerLocation));

	AChaserEnemy* ChaserClass = Cast<AChaserEnemy>(AICharacter);

	const float ChaserAttackRange = ChaserClass->GetAttackRange();

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,FString::Printf(TEXT("Chaser Range %f"), ChaserAttackRange));
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Red,FString::Printf(TEXT("Distance %f"), Distance));
	
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,FString::Printf(TEXT("%f"), Distance));
	if(Distance >= ChaserAttackRange)
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Not Close"));
		BlackBoard->SetValueAsBool(BBKey_IsPlayerClose.SelectedKeyName, false);
		return;
	}

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("Close"));
	BlackBoard->SetValueAsBool(BBKey_IsPlayerClose.SelectedKeyName, true);
}
