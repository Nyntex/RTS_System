// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityFunctions.generated.h"

/**
 * 
 */
UCLASS(meta=(ScriptName="UtilityFunctions"))
class RTS_SYSTEM_API UUtilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Vector", meta=(ReturnDisplayName="NewPosition"))
	static FVector ConvertPositionToBuildingPosition(const FVector original,const float buildingSize);

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf ="WorldContextObject"))
	static FVector LineTraceFromMouseToWorld(bool& Hit, FHitResult& OutHit, AActor*& HitActor, UObject* WorldContextObject, float MaxDistance = 1000000);

	//This is a different Variant of AHUD::GetActorsInSelectionRectangle 
	UFUNCTION(BlueprintCallable, meta=(DeprecatedFunction))
	static void GetMarqueeSelection(UObject* WorldContextObject, const FVector2D FirstPoint, const FVector2D SecondPoint, TArray<AActor*>& OutActors, bool bIncludeNonCollidingComponents, bool bActorMustBeFullyEnclosed);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (AutoCreateRefTerm = "CurrentSelection"))
	static void GetHighestPrioritySelection(TArray<AActor*> CurrentSelection, TArray<AActor*>& NewSelection);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(AutoCreateRefTerm="OriginalSelection"))
	static void ConvertToSingleSelectables(TArray<AActor*> OriginalSelection, TArray<AActor*>& NewSelection);
};
