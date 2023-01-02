// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Fireball.generated.h"

UCLASS()
class UNREAL_PROJECT_BASE_API AFireball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireball();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* Collider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Plane;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileMovementComponent* ProjMoveComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	URadialForceComponent* RadialForceComp;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
