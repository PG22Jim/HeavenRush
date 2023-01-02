// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class Chaser_AIState : uint8
{
 Back UMETA(DisplayName = "BACK"),
 Front UMETA(DisplayName = "FRONT"),
 None 
};
