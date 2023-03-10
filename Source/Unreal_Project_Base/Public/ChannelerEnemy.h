// Copyright © 2022 PotatoFive, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "PoisonArea.h"
#include "ChannelerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_PROJECT_BASE_API AChannelerEnemy : public ABaseEnemy
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void SaveActivatedArea(APoisonArea* PoisonArea);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PrepareToRecast();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APoisonArea*> PoisonAreas;

public:
	UFUNCTION(BlueprintCallable)
	void DestroyPoisonAreas();
	
	
};

inline void AChannelerEnemy::SaveActivatedArea(APoisonArea* NewPoisonArea)
{
	PoisonAreas.Add(NewPoisonArea);
}

inline void AChannelerEnemy::DestroyPoisonAreas()
{
	for (APoisonArea* PoisonArea : PoisonAreas )
	{
		PoisonArea->EraseAddedViewport();
		PoisonArea->Destroy();
	}
}

