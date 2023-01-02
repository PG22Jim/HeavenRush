


#include "BTTask_GetBypassPos.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"






EBTNodeResult::Type UBTTask_GetBypassPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	AAIController* AiController = OwnerComp.GetAIOwner();
	if(AiController == nullptr) return EBTNodeResult::Failed;

	ACharacter* AiCharacter = AiController->GetCharacter();
	if(AiCharacter == nullptr) return  EBTNodeResult::Failed;

	const FVector BypassDestination = GetPositionToMove(AiCharacter);
	
	// const int32 InvalidIndex = BlackBoard->GetValueAsEnum(InValidDirectionToBypass.SelectedKeyName);
	//
	// const FVector BypassDestination = PlaceToMove(InvalidIndex, AiCharacter, BlackBoard);

	BlackBoard->SetValueAsVector(MoveToPosition.SelectedKeyName, BypassDestination);
	
	return EBTNodeResult::Succeeded;
}


FVector UBTTask_GetBypassPos::GetPositionToMove(ACharacter* ChaserRef)
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return {0,0,0};

	const FVector Pos = ChaserRef->GetActorLocation();
	const FRotator Rot = ChaserRef->GetActorRotation();

	AChaserEnemy* CurrentChaserClass = Cast<AChaserEnemy>(ChaserRef);
	
	const FVector ActorRightSideDirection =	ChaserRef->GetActorRightVector();

	const FSideAngleInfo LeftAlphaInfo = SideAngleAlpha(true, Pos, Rot, ActorRightSideDirection, 0.3);
	const FSideAngleInfo RightAlphaInfo = SideAngleAlpha(false, Pos, Rot, ActorRightSideDirection, 0.3);

	if(LeftAlphaInfo.GetSideAlpha() > 1 && RightAlphaInfo.GetSideAlpha() > 1) return {0,0,0};
	
	if(LeftAlphaInfo.GetSideAlpha() < RightAlphaInfo.GetSideAlpha())
	{
		CurrentChaserClass->SetIsVerticalByPass(LeftAlphaInfo.GetIsVerticalDestination());
		if(LeftAlphaInfo.GetIsVerticalDestination()) CurrentChaserClass->SetIsByPassingDoneOnce(true);
		return LeftAlphaInfo.GetSideDestination();
	}

	CurrentChaserClass->SetIsVerticalByPass(RightAlphaInfo.GetIsVerticalDestination());
	if(LeftAlphaInfo.GetIsVerticalDestination()) CurrentChaserClass->SetIsByPassingDoneOnce(true);
	return RightAlphaInfo.GetSideDestination();
}


FSideAngleInfo UBTTask_GetBypassPos::SideAngleAlpha(bool bIsLeft, FVector ChaserPosition, FRotator FacingRotation,  FVector RightVector, float CurrentAlpha)
{
	FSideAngleInfo EmptyInfo;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return EmptyInfo;
	
	// Grab the lerped rotation as current checking side angle;

	FVector SideVector = RightVector;
	
	if(bIsLeft) SideVector *= -1;
	const FVector SideOffset = SideVector * ByPassDisplacement;
	const FVector SideEndPoint = ChaserPosition + SideOffset;
	// Object blocking checking
	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);
	IgnoreActors.Add(PlayerCharacter);

	//UKismetSystemLibrary::LineTraceSingle(World,ChaserPosition, SideEndPoint,UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::Persistent, Hit, true);
	
	
	const FRotator SideRotation = UKismetMathLibrary::FindLookAtRotation(ChaserPosition,SideEndPoint);

	const FRotator AlphaRotation = UKismetMathLibrary::RLerp(FacingRotation, SideRotation, CurrentAlpha, true);

	const FVector AlphaDirection = UKismetMathLibrary::GetForwardVector(AlphaRotation);

	const FVector TraceDestination = ChaserPosition + (AlphaDirection * ByPassDisplacement);

	
	const FVector TraceUpStart = ChaserPosition + FVector{0,0,ByPassVerticalOffset};
	const FVector TraceUpDest = TraceDestination + FVector{0,0,ByPassVerticalOffset};

	
	const FVector TraceDownStart = ChaserPosition - FVector{0,0,ByPassVerticalOffset};
	const FVector TraceDownDest = TraceDestination - FVector{0,0,ByPassVerticalOffset};
	
	
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(World,ChaserPosition, TraceDestination, 60,UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	
	
	if(!bHit)
	{
		FSideAngleInfo ReturnInfo = FSideAngleInfo(CurrentAlpha, Hit.TraceEnd, false);
		return ReturnInfo;
	}

	bHit = UKismetSystemLibrary::SphereTraceSingle(World,TraceUpStart, TraceUpDest, 60,UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(!bHit)
	{
		FSideAngleInfo ReturnInfo = FSideAngleInfo(CurrentAlpha, TraceUpStart, true);
		return ReturnInfo;
	}
	
	bHit = UKismetSystemLibrary::SphereTraceSingle(World,TraceDownStart, TraceDownDest, 60,UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	
	if(!bHit)
	{
		FSideAngleInfo ReturnInfo = FSideAngleInfo(CurrentAlpha, TraceUpStart, true);
		return ReturnInfo;
	}
	
	
	
	const float NextAlpha = CurrentAlpha + 0.1;

	
	if(NextAlpha < 1) return SideAngleAlpha(bIsLeft, ChaserPosition, FacingRotation, RightVector, NextAlpha);

	return EmptyInfo;
}




FVector UBTTask_GetBypassPos::PlaceToMove(const int32 InvalidNum, const ACharacter* AICharacter, UBlackboardComponent* BBComp)
{
	int32 DirIndex = 0;
	bool IsDecided = false;

	while (!IsDecided)
	{
		// 0-3
		int32 RndNum = UKismetMathLibrary::RandomInteger(4);
		if(RndNum != InvalidNum)
		{
			DirIndex = RndNum;
			IsDecided = true;
		}
	}
	
	FVector Offset = {0,0,0};
	FVector AILocation = AICharacter->GetActorLocation();
	
	switch (DirIndex)
	{
		case 0:
			Offset.Z = ByPassDisplacement;
			BBComp->SetValueAsEnum(InValidDirectionToBypass.SelectedKeyName, 1);
			break;
		case 1:
			Offset.Z = -(ByPassDisplacement);
			BBComp->SetValueAsEnum(InValidDirectionToBypass.SelectedKeyName, 0);
			break;
		case 2:
			Offset = (-(AICharacter->GetActorRightVector())) * ByPassDisplacement;
			BBComp->SetValueAsEnum(InValidDirectionToBypass.SelectedKeyName, 3);
			break;
		case 3:
			Offset = (AICharacter->GetActorRightVector()) * ByPassDisplacement;
			BBComp->SetValueAsEnum(InValidDirectionToBypass.SelectedKeyName, 2);
			break;
		default:
			BBComp->SetValueAsEnum(InValidDirectionToBypass.SelectedKeyName, 4);
			break;
	}

	return AILocation + Offset;
}
