// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseEnemy::StopRotation()
{
	RotateTimeline.Stop();
	RotateFinish();
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	SetGlowingMaterialInst();

	// Rotate TimeLine
	FOnTimelineFloat RotateProgressUpdate;
	RotateProgressUpdate.BindDynamic(this, &ABaseEnemy::RotateUpdate);

	FOnTimelineEvent RotateProgressFinish;
	RotateProgressFinish.BindDynamic(this, &ABaseEnemy::RotateFinish);

	RotateTimeline.AddInterpFloat(RotateTimelineCurve, RotateProgressUpdate);
	RotateTimeline.SetTimelineFinishedFunc(RotateProgressFinish);

	// Dissolving Timeline
	FOnTimelineFloat DeathProgressUpdate;
	DeathProgressUpdate.BindDynamic(this, &ABaseEnemy::DissolveUpdate);

	FOnTimelineEvent DeathProgressFinish;
	DeathProgressFinish.BindDynamic(this, &ABaseEnemy::DissolveFinish);

	DeathTimeline.AddInterpFloat(DissolveCurveFloat, DeathProgressUpdate);
	DeathTimeline.SetTimelineFinishedFunc(DeathProgressFinish);

	// Map Entering Timeline
	FOnTimelineFloat EnteringProgressUpdate;
	EnteringProgressUpdate.BindDynamic(this, &ABaseEnemy::EnteringUpdate);

	FOnTimelineEvent EnteringProgressFinish;
	EnteringProgressFinish.BindDynamic(this, &ABaseEnemy::EnteringFinish);

	EnteringTimeline.AddInterpFloat(EnteringCurveFloat, EnteringProgressUpdate);
	EnteringTimeline.SetTimelineFinishedFunc(EnteringProgressFinish);
	

	StopBehaviourTree();
	
	SpawnMagicCircle();

	IsCanBeAttacked = false;

	FVector NewColor = {1,0,0};
	SetEmissiveColor(NewColor);
	
	EnteringTimeline.PlayFromStart();
	
	OnTakeAnyDamage.AddDynamic(this, &ABaseEnemy::OnDamage);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTimeline.TickTimeline(DeltaTime);
	DeathTimeline.TickTimeline(DeltaTime);
	EnteringTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::TryRotateTo(float Current, float Next)
{
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	const float Difference = Next - Current;
	const AController* AiController = GetController();
	const FRotator CurrentRotation = GetActorRotation();
	NextRotation = FRotator{CurrentRotation.Pitch, CurrentRotation.Yaw + Difference, CurrentRotation.Roll};

	IsRotating = true;
	RotateTimeline.PlayFromStart();
}


void ABaseEnemy::RotateUpdate(float Alpha)
{
	const FRotator CurrentRotation = GetActorRotation();

	const FRotator DestinationRotation = UKismetMathLibrary::RLerp(CurrentRotation,NextRotation ,Alpha, true);

	SetActorRotation(DestinationRotation);

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE,3.0f,FColor::Yellow,FString::Printf(TEXT("Updating Rotation %s"), *AiController->GetControlRotation().ToString()));
}

void ABaseEnemy::RotateFinish()
{
	CurrentRotateDegree = OverwritingDegree;
	IsRotating = false;

	UCapsuleComponent* EnemyCapsule = GetCapsuleComponent();
	if(EnemyCapsule->IsSimulatingPhysics())
	{
		DisablePhysics();
		ContinueBehaviourTree();
	}
}

void ABaseEnemy::OnDissolving()
{
	StopBehaviourTree();
	StartPlayDissolving();
}

void ABaseEnemy::OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	//StopBehaviourTree();
	
	WidgetHitMarker();
	TriggerDamageEffect(DamageType);

	const float DecreasedHealth = Health - Damage;
	ReceiveDamageEvent(DecreasedHealth);
	if(DecreasedHealth <= 0)
	{
		if(IsDead) return;
		
		IsDead = true;
		DisableCollision();
		OnDeath();
		return;
	}

	Health = DecreasedHealth;
}

void ABaseEnemy::ReceiveDamageEvent(float DecreasedDamage)
{
	StopBehaviourTree();
	if(DecreasedDamage > 0) ContinueBehaviourTree();
}


void ABaseEnemy::StopBehaviourTree()
{
	AController* AIController = GetController();
	ABaseEnemyAIController* BaseAIController = Cast<ABaseEnemyAIController>(AIController);
	if(BaseAIController == nullptr) return;

	BaseAIController->BrainComponent->StopLogic("Dead");
}

void ABaseEnemy::ContinueBehaviourTree()
{
	AController* AIController = GetController();
	ABaseEnemyAIController* BaseAIController = Cast<ABaseEnemyAIController>(AIController);
	if(BaseAIController == nullptr) return;

	BaseAIController->BrainComponent->StartLogic();
}

void ABaseEnemy::StartPlayDissolving()
{
	DeathTimeline.PlayFromStart();
}

void ABaseEnemy::DisablePhysics()
{
	UCapsuleComponent* EnemyCapsule = GetCapsuleComponent();
	EnemyCapsule->SetSimulatePhysics(false);
}

void ABaseEnemy::StopChaserMovement()
{
}

void ABaseEnemy::DissolveUpdate(float Alpha)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();

	CharacterMesh->SetScalarParameterValueOnMaterials("Dissolve", Alpha);
}

void ABaseEnemy::DissolveFinish()
{
	Destroy();
}

void ABaseEnemy::SetEmissiveColor(FVector NewColor)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();

	CharacterMesh->SetVectorParameterValueOnMaterials("Emissive Color", NewColor);
}

void ABaseEnemy::ReSetEmissiveColor()
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();

	FVector NewColor = {1,1,0};
	CharacterMesh->SetVectorParameterValueOnMaterials("Emissive Color", NewColor);
}

void ABaseEnemy::EnteringUpdate(float Alpha)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	CharacterMesh->SetScalarParameterValueOnMaterials("Dissolve", Alpha);
}

void ABaseEnemy::EnteringFinish()
{
	IsCanBeAttacked = true;
	ReSetEmissiveColor();
	ContinueBehaviourTree();
}

void ABaseEnemy::EnablePhysics()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	UCapsuleComponent* EnemyCapsule = GetCapsuleComponent();
	EnemyCapsule->SetSimulatePhysics(true);

	RotateTimeline.Stop();
	StopChaserMovement();
	World->GetTimerManager().SetTimer(ResetPhysicsTimer,this, &ABaseEnemy::ResetRotation, 0.5, false, 0);
	
}

void ABaseEnemy::SetGlowingMaterialInst()
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	UMaterialInterface* GlowMaterial = CharacterMesh->GetMaterial(GlowingMaterialIndex);
	GlowingMaterialInstanceREF = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,GlowMaterial);
	CharacterMesh->SetMaterial(GlowingMaterialIndex,GlowingMaterialInstanceREF);
}

void ABaseEnemy::SetIsNotAttacking()
{
	UBlackboardComponent* BlackBoard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	if(BlackBoard == nullptr) return;

	BlackBoard->SetValueAsBool("IsAttacking", false);
}

bool ABaseEnemy::IsKillHit(float Damage)
{
	if(Health <= Damage)
		return true;
	
	return false;
}


void ABaseEnemy::ResetRotation()
{
	if(IsDead) return;
	
	NextRotation = {0,0,0};

	RotateTimeline.PlayFromStart();
}



