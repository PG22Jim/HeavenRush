// Copyright © 2022 PotatoFive, All Rights Reserved

#include "MapManager.h"

// Sets default values
AMapManager::AMapManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}