// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityFunctions.generated.h"

/**
 * 
 */
UCLASS()
class RTS_SYSTEM_API UUtilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector ConvertPositionToBuildingPosition(FVector original, float buildingSize);

	UFUNCTION(BlueprintCallable)
	static FVector LineTraceFromMouseToWorld(UObject* WorldContextObject, float MaxDistance, AActor*& HitActor, bool& Hit, FHitResult& OutHit);
};
