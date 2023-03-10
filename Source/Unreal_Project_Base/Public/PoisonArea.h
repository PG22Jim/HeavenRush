// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PoisonArea.generated.h"

class AChannelerEnemy;
UCLASS()
class UNREAL_PROJECT_BASE_API APoisonArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoisonArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly,BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* AreaMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelingSetting)
	AChannelerEnemy* CasterREF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelingSetting)
	float ChannelTime = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelingSetting)
	bool IsChannelingDone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ExplosionSetting)
	float ExplosionRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ExplosionSetting)
	float ExplosionDamage = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ExplosionSetting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ChannelingSetting)
	UMaterialInstance* PoisonMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MaterialSetting)
	UMaterialInstanceDynamic* MaterialREF;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MaterialSetting)
	int32 MaterialIndex = 0;
	
	UFUNCTION(BlueprintImplementableEvent)
	void DealExplosionDamage(APlayerCharacter* DamagedCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void Casting();

	
	UFUNCTION(BlueprintImplementableEvent)
	void SetMaterialREF();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnExplosionEffect();
	
	UFUNCTION(BlueprintCallable)
	void Explosion();
	
	FTimerHandle ChannelingTimer;

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void EraseAddedViewport();

private:
	TArray<AActor*> ExplosionCheck();
	void FinishChanneling();
	
};

inline APoisonArea::APoisonArea()
{
	AreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaMesh"));
	SetRootComponent(AreaMesh);

}

inline void APoisonArea::BeginPlay()
{
	Super::BeginPlay();

	SetMaterialREF();
	Casting();
	
	// const UWorld* World = GetWorld();
	// if(World == nullptr) return;
	//
	// World->GetTimerManager().SetTimer(ChannelingTimer, this, &APoisonArea::FinishChanneling , ChannelTime, false, -1);
}

inline void APoisonArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

inline void APoisonArea::Explosion()
{
	SpawnExplosionEffect();
	TArray<AActor*> ActorsInArea = ExplosionCheck();

	for( AActor* IteratedActor : ActorsInArea)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(IteratedActor);
		if(PlayerCharacter == nullptr) continue;
		DealExplosionDamage(PlayerCharacter);
	}

	EraseAddedViewport();
	Destroy();
}


inline TArray<AActor*> APoisonArea::ExplosionCheck()
{
	// Hit result
	TArray<AActor*> ActorsInRange;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return ActorsInRange;
	
	// Empty array of ignoring actor
	const TArray<AActor*> IgnoreActors;
	
	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), ExplosionRadius, FilterType, AActor::StaticClass(), IgnoreActors, ActorsInRange);
	return ActorsInRange;
}

inline void APoisonArea::FinishChanneling()
{
	IsChannelingDone = true;
	AreaMesh->SetMaterial(0,PoisonMaterial);
	Explosion();
}
