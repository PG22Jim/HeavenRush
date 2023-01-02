// Fill out your copyright notice in the Description page of Project Settings.


#include "Fireball.h"

// Sets default values
AFireball::AFireball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	ProjMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Component"));

	SetRootComponent(Collider);
	Plane->SetupAttachment(Collider);
	RadialForceComp->SetupAttachment(Collider);
	
	ProjMoveComp->InitialSpeed = 3000;
	ProjMoveComp->MaxSpeed = 3000;
	ProjMoveComp->ProjectileGravityScale = 0.0f;
	
	RadialForceComp->ImpulseStrength = 100000.0f;
	
}

// Called when the game starts or when spawned
void AFireball::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

