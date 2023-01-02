// Copyright © 2022 PotatoFive, All Rights Reserved

#include "AIManager.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAIManager::AAIManager()
{
	
}

void AAIManager::BeginPlay()
{
	Super::BeginPlay();

	InitializePlayerReference();
}

void AAIManager::MapInitialization()
{
	// for (int i = 0; i < ChaserSpawnAmount; i++)
	// {
	// 	
	// }
}

void AAIManager::DestroyAllEnemy()
{
    if(CurrentChasersInMap.Num() > 0)
    {
		for (AChaserEnemy* EachChaser : CurrentChasersInMap )
		{
			if(EachChaser != nullptr)
			{
				EachChaser->Destroy();
				EachChaser = nullptr;
			}
		}
	    CurrentChasersInMap.Empty();
    }
	if(CurrentChannelerInMap.Num() > 0)
	{
		for (AChannelerEnemy* EachChannelerEnemy : CurrentChannelerInMap )
		{
			if(EachChannelerEnemy != nullptr)
			{
				EachChannelerEnemy->Destroy();
				EachChannelerEnemy = nullptr;
			}
		}
		CurrentChannelerInMap.Empty();
	}
	
	if(CurrentShooterInMap.Num() > 0)
	{
		for (AShooterEnemy* EachShooter : CurrentShooterInMap )
		{
			if(EachShooter != nullptr)
			{
				EachShooter->Destroy();
				EachShooter = nullptr;
			}
		}
		CurrentShooterInMap.Empty();
	}
}

void AAIManager::SwitchMapPhase(TEnumAsByte<GameMapPhase> NextMapPhase)
{
	if(CurrentMapPhase == NextMapPhase) return;

	switch (NextMapPhase)
	{
	case Map1:
		CurrentMapPhase = Map1;
		CurrentChaserSpawnLocations = Map1ChaserSpawnLocations;
		CurrentChannelerSpawnLocations = Map1ChannelerSpawnLocations;
		CurrentShooterSpawnLocations = Map1ShooterSpawnLocations;
		
		break;
	case Map2:
		CurrentMapPhase = Map2;
		CurrentChaserSpawnLocations = Map2ChaserSpawnLocations;
		CurrentChannelerSpawnLocations = Map2ChannelerSpawnLocations;
		CurrentShooterSpawnLocations = Map2ShooterSpawnLocations;
		
		break;
	case Map3:
		CurrentMapPhase = Map3;
		CurrentChaserSpawnLocations = Map3ChaserSpawnLocations;
		CurrentChannelerSpawnLocations = Map3ChannelerSpawnLocations;
		CurrentShooterSpawnLocations = Map3ShooterSpawnLocations;
			
		break;
	default:
		break;
	}
}

void AAIManager::KillAllEnemiesInMap()
{
	if(CurrentChasersInMap.Num() > 0)
	{
		for (AChaserEnemy* EachChaser : CurrentChasersInMap )
		{
			if(EachChaser != nullptr)
			{
				DealDamageTo(EachChaser);
			}
		}
	}
	if(CurrentChannelerInMap.Num() > 0)
	{
		for (AChannelerEnemy* EachChannelerEnemy : CurrentChannelerInMap )
		{
			if(EachChannelerEnemy != nullptr)
			{
				DealDamageTo(EachChannelerEnemy);
			}
		}
	}
	
	if(CurrentShooterInMap.Num() > 0)
	{
		for (AShooterEnemy* EachShooter : CurrentShooterInMap )
		{
			if(EachShooter != nullptr)
			{
				DealDamageTo(EachShooter);
			}
		}
	}
}


// ============================================ Threat System ============================================
void AAIManager::SetChasersBasedOnThreat()
{
	// Get Chasers in Range
	const TArray<AChaserEnemy*> ChasersInRange = GetAllChasersInCloseRange(EnemyCanAttackRadius);

	if(ChasersInRange.Num() == 0)
		return;

	const int32 HighestThreatLevel = FindHighestThreatLevel(ChasersInRange);
	
	// Get Only chasers with highest threat level
	const TArray<AChaserEnemy*> HighThreatChasers = GetChasersWithHighestThreatLevel(ChasersInRange, HighestThreatLevel);

	//
	TArray<AChaserEnemy*> AttackingChasers = GetChasersToAttack(HighThreatChasers);

	if(AttackingChasers.Num() == 0)
		return;

	SetChaserThreatLevel(ChasersInRange);
	
	SetChasersToAttack(AttackingChasers);

}

void AAIManager::InitializePlayerReference()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	ACharacter* ReturnedCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);

	APlayerCharacter* CastedPlayerCharacter = Cast<APlayerCharacter>(ReturnedCharacter);

	if(CastedPlayerCharacter != nullptr) PlayerCharacterClass = CastedPlayerCharacter;
}

TArray<AChaserEnemy*> AAIManager::GetAllChasersInCloseRange(float TraceRadius)
{
	// Create Empty Array of returning chasers
	TArray<AChaserEnemy*> ReturnChasers;

	// Get World 
	const UWorld* World = GetWorld();
	if(World == nullptr) return ReturnChasers;

	// GetPlayer's location
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World,0);
	const FVector PlayerPos = PlayerCharacter->GetActorLocation();

	// Get Actors of Chasers in range
	TArray<AActor*> ChaserActors;
	TArray<AActor*> IgnoreActors;
	const bool bIsChaserInRange = UKismetSystemLibrary::SphereOverlapActors(World,PlayerPos,TraceRadius,FilterType,ChaserClass, IgnoreActors,ChaserActors);

	// If there is no enemy, just return the empty array
	if(!bIsChaserInRange) return ReturnChasers;

	// convert AActor* to AChaserEnemy*
	for (AActor* EachChaserActor : ChaserActors)
	{
		AChaserEnemy* TheChaserClass = Cast<AChaserEnemy>(EachChaserActor);
		if(TheChaserClass != nullptr)
			ReturnChasers.Add(TheChaserClass);
	}

	return ReturnChasers;
}

int32 AAIManager::FindHighestThreatLevel(TArray<AChaserEnemy*> ChasersInCloseRange)
{
	int32 ThreatLevel = 1;

	for (AChaserEnemy* EachChaser : ChasersInCloseRange)
	{
		const int32 ChaserThreat = EachChaser->GetThreatLevel();
		if(ChaserThreat > ThreatLevel)
			ThreatLevel = ChaserThreat;
	}
	
	return ThreatLevel;
}

TArray<AChaserEnemy*> AAIManager::GetChasersWithHighestThreatLevel(TArray<AChaserEnemy*> ChasersInCloseRange, int32 HighestThreat)
{
	// Empty Array of Highest level chasers
	TArray<AChaserEnemy*> HighThreatChasers;

	// For each chaser
	for (AChaserEnemy* Chaser : ChasersInCloseRange)
	{
		// check if chaser is null ptr, if it is. continue
		if(Chaser == nullptr) continue;
		
		// Add into array if chaser is in highest threat level
		if(Chaser->GetThreatLevel() >= HighestThreat)
		{
			HighThreatChasers.Add(Chaser);
		}
	}
	
	return HighThreatChasers;
}

TArray<AChaserEnemy*> AAIManager::GetChasersToAttack(TArray<AChaserEnemy*> ChasersWithHighestLevel)
{
	// if number of chaser can attack is smaller than num to set attack, just return it as chasers to attack;
	if(ChasersWithHighestLevel.Num() < MaxNumForEachAttack)
	{
		for (AChaserEnemy* EachChaser : ChasersWithHighestLevel)
		{
			EachChaser->SetIsAssignedToAttack(true);
		}
		return ChasersWithHighestLevel;
	}

	// Empty arrays of chasers to attack
	TArray<AChaserEnemy*> ChasersToAttack;

	// for loop to get (MaxNumForEachAttack)X enemies to attack
	for(int i = 0; i < MaxNumForEachAttack; i++)
	{
		// while loop until find chaser that haven't set into attack list
		bool bIsChaserChose = false;
		while(!bIsChaserChose)
		{
			// grab random chaser in the high threat list and check if it is not in the list yet
			const int32 RndNum = UKismetMathLibrary::RandomInteger(ChasersWithHighestLevel.Num());
			if(!ChasersWithHighestLevel[RndNum]->GetIsAssignedToAttack())
			{
				ChasersWithHighestLevel[RndNum]->SetIsAssignedToAttack(true);
				ChasersToAttack.Add(ChasersWithHighestLevel[RndNum]);
				bIsChaserChose = true;
			}
		}
	}
	
	return ChasersToAttack;
}

void AAIManager::SetChasersToAttack(TArray<AChaserEnemy*> ChasersToAttack)
{
	// Set chasers to attack and make the assign bool as false
	for(AChaserEnemy* Chaser : ChasersToAttack)
	{
		Chaser->SetCanAttack();
		Chaser->SetIsAssignedToAttack(false);
	}
}

void AAIManager::SetChaserThreatLevel(TArray<AChaserEnemy*> Chasers)
{
	for (AChaserEnemy* EachChaser : Chasers)
	{
		// Avoid having nullptr chaser
		if(EachChaser == nullptr) continue;

		// if this chaser is the one to attack, reset threat level
		if(EachChaser->GetIsAssignedToAttack())
			EachChaser->ResetThreatLevel();
		else
		{
			// if this chaser is not attacking, increase threat level
			EachChaser->IncreaseThreatLevel();
		}
	}
}

void AAIManager::SetHighestThreatLevel(int32 ThreatLevel)
{
	int32 PlaceHolderInt = 1;
	// overwrite threat level only when chaser's threat level is higher than current highest threat level
	if(PlaceHolderInt < ThreatLevel)
		PlaceHolderInt = ThreatLevel;
}



// ========================================= Teleportation System =========================================

void AAIManager::TeleportChasersToFront()
{
	// Get All Chasers in Range
	const TArray<AChaserEnemy*> ChasersInRange = GetAllChasersInCloseRange(ChaserTeleportationRadius);
	
	if(ChasersInRange.Num() == 0) return;
	
	// Get All Chasers At player's back
	TArray<AChaserEnemy*> ChasersAtBack = GetChaserAtPlayerBack(ChasersInRange);

	// if number of chasers at back is less than requirement, no need to teleport
	const int32 NumOfChaserAtBack = ChasersAtBack.Num();
	if(NumOfChaserAtBack < NumOfChaserAtBackRequirement) return;
	
	// Get Specific Point to Spawn
	const FVector SPLocation = GetChaserSpawnPoint();

	// fot loop to teleport each chaser to ideal spawn point
	for (int32 i = NumOfChaserRemainAtBack; i < ChasersAtBack.Num(); i++)
	{
		// Stop chaser's movement and teleport it to front
		AChaserEnemy* IteratingChaser = ChasersAtBack[i];
		IteratingChaser->StartTeleportingTo(SPLocation);
	}
	
}


TArray<AChaserEnemy*> AAIManager::GetChaserAtPlayerBack(TArray<AChaserEnemy*> ChasersInRange)
{
	TArray<AChaserEnemy*> ReturnChasers;
	
	if(PlayerCharacterClass == nullptr) return ReturnChasers;

	const FVector PlayerMovingDir = PlayerCharacterClass->GetActorForwardVector();

	for( AChaserEnemy* EachChaser : ChasersInRange)
	{
		FVector ChaserLocationWithoutZ = EachChaser->GetActorLocation();
		ChaserLocationWithoutZ.Z = 0;
		
		FVector PlayerLocation = PlayerCharacterClass->GetActorLocation();
		PlayerLocation.Z = 0;

		const FVector DirectionToPlayer = UKismetMathLibrary::Normal((PlayerLocation - ChaserLocationWithoutZ));
		
		const float DotProduct = UKismetMathLibrary::Dot_VectorVector(PlayerMovingDir, DirectionToPlayer);
		
		if(DotProduct > 0.2) ReturnChasers.Add(EachChaser);
	}
	
	return ReturnChasers;
}

FVector AAIManager::GetChaserSpawnPoint()
{
	const FVector PlayerMovingDir = PlayerCharacterClass->GetActorForwardVector();
	
	FVector PlayerLocation = PlayerCharacterClass->GetActorLocation();
	PlayerLocation.Z = 0;

	// making an array of spawn point info, containing pointer and dot product 
	TArray<FSpawnPointInfo> PointInfos;

	// for each chaser spawn points
	for (AChaserSpawnPoint* EachPoint : CurrentChaserSpawnLocations)
	{
		// Find direction from player to spawn point
		FVector PointPosWithoutZ = EachPoint->GetActorLocation();
		PointPosWithoutZ.Z = 0;
		
		const FVector DirectionToSpawnPoint = UKismetMathLibrary::Normal((PointPosWithoutZ - PlayerLocation));
		
		// Get Dot product with player's moving direction and direction from player to spawn point
		const float DotProduct = UKismetMathLibrary::Dot_VectorVector(PlayerMovingDir, DirectionToSpawnPoint);

		// Create Info and add into it
		FSpawnPointInfo Info = FSpawnPointInfo(DotProduct, EachPoint->GetActorLocation());
		PointInfos.Add(Info);
	}

	if(PointInfos.Num() < 1) return {0,0,0};
	
	// Quick sort the list of chaser spawn point info
	ChaserSpawnPointInfos = PointInfos;
	
	StartChaserSPQuickSort(ChaserSpawnPointInfos);
	

	// find the best spawn point that is at player's front and cannot be seen
	FSpawnPointInfo ResultSPInfo = ChaserSpawnPointIteration(ChaserSpawnPointInfos, true);

	// if the result's dot product is minus, it means the function cannot find the right one and return the last one in the sorted list or only spawn point that cannot be seen are behind player
	if(ResultSPInfo.GetDotProductValue() < 0)
	{
		// this time, just get the first one in the list
		ResultSPInfo = ChaserSpawnPointIteration(ChaserSpawnPointInfos, false);
	}
	
	return ResultSPInfo.GetSpawnPointPos();
}

TArray<FSpawnPointInfo> AAIManager::GetListOfSpawnPointInfos(FVector PlayerLocation, FVector PlayerMovingDir, int32 Index)
{
	// making an array of spawn point info, containing pointer and dot product 
	TArray<FSpawnPointInfo> ReturnPointInfos;


	switch (Index)
	{
		case 1:
			// for each chaser spawn points
			for (AChaserSpawnPoint* EachPoint : CurrentChaserSpawnLocations)
			{
				// Find direction from player to spawn point
				FVector PointPosWithoutZ = EachPoint->GetActorLocation();
				PointPosWithoutZ.Z = 0;
		
				const FVector DirectionToSpawnPoint = UKismetMathLibrary::Normal((PointPosWithoutZ - PlayerLocation));
		
				// Get Dot product with player's moving direction and direction from player to spawn point
				const float DotProduct = UKismetMathLibrary::Dot_VectorVector(PlayerMovingDir, DirectionToSpawnPoint);
				
				// Create Info and add into it
				FSpawnPointInfo Info = FSpawnPointInfo(DotProduct, EachPoint->GetActorLocation());
				ReturnPointInfos.Add(Info);
			}
			break;
		case 2:
			// for each caster spawn points
			for (AChannelerSpawnPoint* EachPoint : CurrentChannelerSpawnLocations)
			{
				// Find direction from player to spawn point
				FVector PointPosWithoutZ = EachPoint->GetActorLocation();
				PointPosWithoutZ.Z = 0;
		
				const FVector DirectionToSpawnPoint = UKismetMathLibrary::Normal((PointPosWithoutZ - PlayerLocation));
		
				// Get Dot product with player's moving direction and direction from player to spawn point
				const float DotProduct = UKismetMathLibrary::Dot_VectorVector(PlayerMovingDir, DirectionToSpawnPoint);
				
				// Create Info and add into it
				FSpawnPointInfo Info = FSpawnPointInfo(DotProduct, EachPoint->GetActorLocation());
				ReturnPointInfos.Add(Info);
			}
			break;
		case 3:
			// for each shooter spawn points
			for (AShooterSpawnPoint* EachPoint : CurrentShooterSpawnLocations)
			{
				// Find direction from player to spawn point
				FVector PointPosWithoutZ = EachPoint->GetActorLocation();
				PointPosWithoutZ.Z = 0;
		
				const FVector DirectionToSpawnPoint = UKismetMathLibrary::Normal((PointPosWithoutZ - PlayerLocation));
		
				// Get Dot product with player's moving direction and direction from player to spawn point
				const float DotProduct = UKismetMathLibrary::Dot_VectorVector(PlayerMovingDir, DirectionToSpawnPoint);


				// Create Info and add into it
				FSpawnPointInfo Info = FSpawnPointInfo(DotProduct, EachPoint->GetActorLocation());
				ReturnPointInfos.Add(Info);
				
			}
			break;
		default:
			break;
	}
	
	return ReturnPointInfos;
}

TArray<FSpawnPointInfo> AAIManager::GetValidSpawnPointList(TArray<FSpawnPointInfo> ListOfSpawnPoint, float DotProductRequire)
{

	TArray<FSpawnPointInfo> ReturnInfos;

	if(DotProductRequire <= -1) return ReturnInfos;
	
	for (FSpawnPointInfo EachInfo: ListOfSpawnPoint)
	{
		if(EachInfo.GetDotProductValue() > DotProductRequire)
			ReturnInfos.Add(EachInfo);
	}

	// early return array of found matching spawn points if result number are not smaller than NumOfSPNeedToHave
	const int32 ResultSpawnPointNumber = ReturnInfos.Num();
	
	if(ResultSpawnPointNumber >= NumOfSPNeedToHave)
	{
		return ReturnInfos;
	}

	// if there is no matching spawn point, lower the requirement with recursive function
	float NextCheckRequire = DotProductRequire - 0.2;
	if(NextCheckRequire < -1) NextCheckRequire = -1;
	return GetValidSpawnPointList(ListOfSpawnPoint, NextCheckRequire);
}

FSpawnPointInfo AAIManager::ChaserSpawnPointIteration(TArray<FSpawnPointInfo> SpawnPointList, bool bNeedToNotSeen)
{

	// if this function need a spawn point that cannot be seen, do for each loop to find one
	if(bNeedToNotSeen)
	{
		// if one of pointer references is null pointer, return last one on the list
		const UWorld* World = GetWorld();
		if(World == nullptr) return SpawnPointList[SpawnPointList.Num()-1];

		ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
		if(PlayerChar == nullptr) return SpawnPointList[SpawnPointList.Num()-1];
		
		// get ignore actor with player characrter
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(PlayerChar);
		
		// get player position
		FVector PlayerPos = PlayerChar->GetActorLocation();
		
		for (FSpawnPointInfo IteratingPointInfo : SpawnPointList)
		{
			// Hit result
			FHitResult Hit;
			
			// Do line ray cast from player's position to spawn point
			const FVector SPLocation = IteratingPointInfo.GetSpawnPointPos();

			const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, PlayerPos, SPLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Hit,true);

			// if hit result, it means something is on the way, player cannot see the spawnpoint
			if(bHit) return IteratingPointInfo;
		}

		// if there is no result, return last one
		return SpawnPointList[SpawnPointList.Num()-1];
	}

	// if don't need to not be seen, return the first one
	return SpawnPointList[0];
}

// ========================================= Utility Spawn Point Finding System =========================================


TArray<FVector> AAIManager::FindIdealSpawnPointPositions(int32 EnemyTypeIndex)
{
	TArray<FVector> ReturnArray;

	const FVector PlayerMovingDir = PlayerCharacterClass->GetActorForwardVector();

	
	FVector PlayerLocation = PlayerCharacterClass->GetActorLocation();
	PlayerLocation.Z = 0;
	
	TArray<FSpawnPointInfo> AllSpawnPointInfos = GetListOfSpawnPointInfos(PlayerLocation, PlayerMovingDir, EnemyTypeIndex);

	const TArray<FSpawnPointInfo> ValidSPInfoList = GetValidSpawnPointList(AllSpawnPointInfos, DotProductRequirementToSpawn);
	
	if(ValidSPInfoList.Num() < 1) return ReturnArray;
	

	for (FSpawnPointInfo EachInfo : ValidSPInfoList)
	{
		ReturnArray.Add(EachInfo.GetSpawnPointPos());
	}
	
	return ReturnArray;
}




// ========================================= Quick Sort =========================================


void AAIManager::StartChaserSPQuickSort(TArray<FSpawnPointInfo> SpawnPointList)
{
	// find the size of list
	const int listSize = SpawnPointList.Num();

	// if list only has 1 or 0 node
	if (listSize <= 1)
	{
		// no need to sort, just return
		return;
	}
	
	// if list size is 2
	// Start sorting with start and end pos of list
	ChaserSPQuickSorting(0, listSize - 1);
}


void AAIManager::ChaserSPQuickSorting(int32 startIndex, int32 endIndex)
{
	// if start is smaller than end, we do sorting
	if (startIndex < endIndex)
	{
		// pivot point become the separated result
		int pivotPos = ChaserSPQuickSortSeparation(startIndex, endIndex);

		// left side sorting
		ChaserSPQuickSorting(startIndex, pivotPos - 1);
		// right side sorting
		ChaserSPQuickSorting(pivotPos + 1, endIndex);
	}
}

int32 AAIManager::ChaserSPQuickSortSeparation(int32 startIndex, int32 endIndex)
{
	// Set pivot point with same mData of our start pos node
	FSpawnPointInfo pivotInfo = ChaserSpawnPointInfos[endIndex];
	// declare the numbers of nodes that are smaller than pivotNode
	int leftSideCount = startIndex;

	// start to compare from start Position
	for (int i = startIndex; i < endIndex; i++)
	{
		// if node of i position is bigger than pivot node
		if (ChaserSpawnPointInfos[i].GetDotProductValue() > pivotInfo.GetDotProductValue())
		{
			// Swap to the left side and increment leftSideCount
			ChaserSpawnPointInfos.Swap(leftSideCount, i);
			leftSideCount++;
		}
	}
	// after iteration, our leftSideCount is the correct postion of our endPos, so switch the place
	ChaserSpawnPointInfos.Swap(leftSideCount,endIndex);
	// return pivot position
	return (leftSideCount);
}



void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
