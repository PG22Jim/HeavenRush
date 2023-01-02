// Copyright © 2022 PotatoFive, All Rights Reserved


#include "BTTask_GetFlyToPosition.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UBTTask_GetFlyToPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);
	const FVector PlayerPosition = PlayerCharacter->GetActorLocation();
	
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return EBTNodeResult::Failed;
	
	const ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return  EBTNodeResult::Failed;
	
	const FVector AiLocation = AiCharacter->GetActorLocation();

	FVector SupposToPosition = GetSupposMoveToPosition(PlayerPosition, AiLocation);

	FVector RealPostionToMove = GetValidPosAroundPlayer(PlayerCharacter, PlayerPosition, SupposToPosition);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,TEXT("I am Here"));
	BlackBoard->SetValueAsVector(BBKey_MoveToPos.SelectedKeyName, RealPostionToMove);

	return EBTNodeResult::Succeeded;
}

FVector UBTTask_GetFlyToPosition::GetSupposMoveToPosition(FVector PlayerPos, FVector AiPosition)
{
	FVector AIPos = FVector{AiPosition.X, AiPosition.Y, PlayerPos.Z };
	const FVector DirectionToAi = UKismetMathLibrary::Normal(AIPos - PlayerPos);
	const FVector OffsetToAi = DirectionToAi * DistanceAroundPlayer;
	return PlayerPos + OffsetToAi;
}

FVector UBTTask_GetFlyToPosition::GetValidPosAroundPlayer(ACharacter* PlayerCharacter, FVector PlayerPos, FVector PositionToMove)
{
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);
	
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, PlayerPos, PositionToMove, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::Persistent,Hit,false);

	if(bHit)
	{
		return Hit.Location;
	}

	return Hit.TraceEnd;
}
