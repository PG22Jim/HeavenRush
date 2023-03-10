// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ChannelerEnemy.h"
#include "ChannelerSpawnPoint.h"
#include "ChaserAIController.h"
#include "ChaserEnemy.h"
#include "ChaserSpawnPoint.h"
#include "ShooterEnemy.h"
#include "ShooterSpawnPoint.h"
#include "GameFramework/Actor.h"
#include "AIManager.generated.h"


UENUM(BlueprintType)
enum GameMapPhase
{
	Map1,
	Map2,
	Map3,
	Map0
};


USTRUCT(BlueprintType)
struct FSpawnPointInfo
{
	GENERATED_BODY()

public:

	FSpawnPointInfo()
	{
		DotProductToSpawnPoint = 0;
		SpawnPointPos = {0,0,0};
	}
	
	FSpawnPointInfo(float DotProduct, FVector Pos)
	{
		DotProductToSpawnPoint = DotProduct;
		SpawnPointPos = Pos;
	}

	float GetDotProductValue() { return DotProductToSpawnPoint; }
	FVector GetSpawnPointPos() { return SpawnPointPos; }

private:
	UPROPERTY()
	float DotProductToSpawnPoint;

	UPROPERTY()
	FVector SpawnPointPos;
};



UCLASS()
class UNREAL_PROJECT_BASE_API AAIManager : public AActor
{
	GENERATED_BODY()

private:

	
public:	
	// Sets default values for this actor's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ======================== Referencing ========================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Reference)
	APlayerCharacter* PlayerCharacterClass;
	

	// ======================== Combat Managing ========================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ThreatSystem)
	TArray<AChaserEnemy*> ChasersWaitingToAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ThreatSystem)
	int32 MaxNumForEachAttack = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ThreatSystem)
	float EnemyCanAttackRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ThreatSystem)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ThreatSystem)
	UClass* ChaserClass;


	// ======================== Chaser Teleportation ========================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ChaserTeleportationSetting)
	float ChaserTeleportCoolDownTime = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ChaserTeleportationSetting)
	float ChaserTeleportationRadius = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ChaserTeleportationSetting)
	int32 NumOfChaserAtBackRequirement = 4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ChaserTeleportationSetting)
	int32 NumOfChaserRemainAtBack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ChaserTeleportationSetting)
	TArray<AChaserSpawnPoint*> ChaserSpawnPoints;

	TArray<FSpawnPointInfo> ChaserSpawnPointInfos;

	
	// ======================== Spawn Picking System ========================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= EnemySPPickingSystem)
	int32 NumOfSPNeedToHave = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= EnemySPPickingSystem)
	float DotProductRequirementToSpawn = 0.65;
	
	TArray<FSpawnPointInfo> EnemySpawnPointInfos;
	
	
	// ======================== Map 1 ========================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap1)
	int32 Map1ChaserSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap1)
	int32 Map1ChannelerSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap1)
	int32 Map1ShooterSpawnAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap1)
	int32 MaxChaserNumMap1 = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap1)
	int32 MaxCasterNumMap1 = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap1)
	int32 MaxShooterNumMap1 = 0;

	
	// ======================== Map 2 ========================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap2)
	int32 Map2ChaserSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap2)
	int32 Map2ChannelerSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap2)
	int32 Map2ShooterSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap2)
	int32 MaxChaserNumMap2 = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap2)
	int32 MaxCasterNumMap2 = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap2)
	int32 MaxShooterNumMap2 = 0;
	
	// ======================== Map 3 ========================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap3)
	int32 Map3ChaserSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap3)
	int32 Map3ChannelerSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap3)
	int32 Map3ShooterSpawnAmount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap3)
	int32 MaxChaserNumMap3 = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap3)
	int32 MaxCasterNumMap3 = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSettingMap3)
	int32 MaxShooterNumMap3 = 0;

	
	// ======================== Map ALL ========================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSetting)
	TEnumAsByte<GameMapPhase> CurrentMapPhase = Map1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSetting)
	int32 TotalEnemiesNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentChaserNum = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentCasterNum = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentShooterNum = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSetting)
	int32 MaxChaserNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSetting)
	int32 MaxCasterNum = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnSetting)
	int32 MaxShooterNum = 0;

	
	
	UFUNCTION(BlueprintCallable)
	void MapInitialization();

	UFUNCTION(BlueprintCallable)
	void DestroyAllEnemy();

	UFUNCTION(BlueprintCallable)
	void SwitchMapPhase(TEnumAsByte<GameMapPhase> NextMapPhase);

	UFUNCTION(BlueprintCallable)
	void KillAllEnemiesInMap();

	UFUNCTION(BlueprintImplementableEvent)
	void DealDamageTo(ABaseEnemy* TargetEnemy);

	UFUNCTION(BlueprintCallable)
	void SetChasersBasedOnThreat();

	UFUNCTION(BlueprintCallable)
	void TeleportChasersToFront();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FVector> FindIdealSpawnPointPositions(int32 EnemyTypeIndex);
	
	// Chaser Spawn Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChaserSpawn)
	TArray<AChaserSpawnPoint*> CurrentChaserSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChaserSpawn)
	TArray<AChaserEnemy*> CurrentChasersInMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChaserSpawn)
	TArray<AChaserSpawnPoint*> Map1ChaserSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChaserSpawn)
	TArray<AChaserSpawnPoint*> Map2ChaserSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChaserSpawn)
	TArray<AChaserSpawnPoint*> Map3ChaserSpawnLocations;
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelerSpawn)
	TArray<AChannelerSpawnPoint*> CurrentChannelerSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelerSpawn)
	TArray<AChannelerEnemy*> CurrentChannelerInMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelerSpawn)
	TArray<AChannelerSpawnPoint*> Map1ChannelerSpawnLocations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelerSpawn)
	TArray<AChannelerSpawnPoint*> Map2ChannelerSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelerSpawn)
	TArray<AChannelerSpawnPoint*> Map3ChannelerSpawnLocations;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShooterSpawn)
	TArray<AShooterSpawnPoint*> CurrentShooterSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShooterSpawn)
	TArray<AShooterEnemy*> CurrentShooterInMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShooterSpawn)
	TArray<AShooterSpawnPoint*> Map1ShooterSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShooterSpawn)
	TArray<AShooterSpawnPoint*> Map2ShooterSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ShooterSpawn)
	TArray<AShooterSpawnPoint*> Map3ShooterSpawnLocations;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TEnumAsByte<GameMapPhase> GetCurrentMapPhase() const { return CurrentMapPhase;}

private:

	void InitializePlayerReference();

	TArray<AChaserEnemy*> GetAllChasersInCloseRange(float TraceRadius);

	int32 FindHighestThreatLevel(TArray<AChaserEnemy*> ChasersInCloseRange);
	
	TArray<AChaserEnemy*> GetChasersWithHighestThreatLevel(TArray<AChaserEnemy*> ChasersInCloseRange, int32 HighestThreat);
	
	TArray<AChaserEnemy*> GetChasersToAttack(TArray<AChaserEnemy*> ChasersWithHighestLevel);
	
	void SetChasersToAttack( TArray<AChaserEnemy*> ChasersToAttack);

	void SetChaserThreatLevel(TArray<AChaserEnemy*> Chasers);

	void SetHighestThreatLevel(int32 ThreatLevel);
	

	TArray<AChaserEnemy*> GetChaserAtPlayerBack(TArray<AChaserEnemy*> ChasersInRange);

	FVector GetChaserSpawnPoint();

	TArray<FSpawnPointInfo> GetListOfSpawnPointInfos(FVector PlayerLocation, FVector PlayerMovingDir, int32 Index); 

	TArray<FSpawnPointInfo> GetValidSpawnPointList(TArray<FSpawnPointInfo> ListOfSpawnPoint, float DotProductRequire);
	
	FSpawnPointInfo ChaserSpawnPointIteration(TArray<FSpawnPointInfo> SpawnPointList, bool bNeedToNotSeen);


	// =============================== Sorting =============================== 
	void StartChaserSPQuickSort(TArray<FSpawnPointInfo> SpawnPointList);
	
	void ChaserSPQuickSorting(int32 startIndex, int32 endIndex);

	int32 ChaserSPQuickSortSeparation(int32 startIndex, int32 endIndex);
};