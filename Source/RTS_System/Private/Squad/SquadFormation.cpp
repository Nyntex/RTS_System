// Fill out your copyright notice in the Description page of Project Settings.


#include "Squad/SquadFormation.h"

FVector USquadFormation::EvaluateLeaderPosition_Implementation(FVector OriginalLocation, USquadComponent* Leader) const
{
	return FVector::Zero();
}

FVector USquadFormation::GetMoveLocationForMember_Implementation(int MemberIndex, FVector OriginalMoveLocation) const
{
	return FVector::Zero();
}
