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

	//This is a different Variant of AHUD::GetActorsInSelectionRectangle 
	UFUNCTION(BlueprintCallable)
	static void GetMarqueeSelection(UObject* WorldContextObject, const FVector2D FirstPoint, const FVector2D SecondPoint, TArray<AActor*>& OutActors, bool bIncludeNonCollidingComponents, bool bActorMustBeFullyEnclosed);
};
