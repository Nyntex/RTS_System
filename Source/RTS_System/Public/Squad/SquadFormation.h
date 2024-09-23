// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SquadComponent.h"
#include "UObject/NoExportTypes.h"
#include "SquadFormation.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent, DisplayName = "SquadFormation"), Blueprintable, NonTransient)
class RTS_SYSTEM_API USquadFormation : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int ValidationInteger = 100;

	UFUNCTION(BlueprintNativeEvent, Category = "SquadComponent")
	FVector GetMoveLocationForMember(int MemberIndex, FVector OriginalMoveLocation) const;

	virtual FVector GetMoveLocationForMember_Internal(int MemberIndex, FVector OriginalMoveLocation) const;

	UFUNCTION(BlueprintNativeEvent, Category = "SquadComponent")
	FVector EvaluateLeaderPosition(FVector OriginalLocation, USquadComponent* Leader) const;

	virtual FVector EvaluateLeaderPosition_Internal(FVector OriginalLocation, USquadComponent* Leader) const;
};
