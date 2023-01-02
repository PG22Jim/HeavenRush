// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetPlayerPos.h"

#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_SetPlayerPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,FString::Printf(TEXT("%s"), *PlayerLocation.ToString()));
	BlackBoard->SetValueAsVector(BBKey_PlayerPos.SelectedKeyName, PlayerLocation);

	
	
}
