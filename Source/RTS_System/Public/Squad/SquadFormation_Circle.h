// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SquadFormation.h"
#include "SquadFormation_Circle.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent, DisplayName = "SquadFormationCircle"), Blueprintable)
class RTS_SYSTEM_API USquadFormation_Circle : public USquadFormation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SquadFormation")
	int DistancePerRing = 250;

	UPROPERTY(EditAnywhere, Category = "SquadFormation")
	int UnitsPerRing = 6;

	virtual FVector EvaluateLeaderPosition_Internal(FVector OriginalLocation, USquadComponent* Leader) const override;

	virtual FVector GetMoveLocationForMember_Internal(int MemberIndex, FVector OriginalMoveLocation) const override;

	UFUNCTION(BlueprintCallable, Category = "SquadFormation")
	void GetRing(int MemberIndex, int& Ring, int& IndexPositionInRing) const;
};
