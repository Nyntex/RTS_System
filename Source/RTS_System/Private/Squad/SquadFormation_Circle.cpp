// Fill out your copyright notice in the Description page of Project Settings.


#include "Squad/SquadFormation_Circle.h"
#include "NavigationSystem.h"


FVector USquadFormation_Circle::EvaluateLeaderPosition_Implementation(FVector OriginalLocation, USquadComponent* Leader) const
{
	int DistanceToCheck = 0;
	{
		int Ring = 0;
		int IndexInRing = 0;
		GetRing(Leader->SquadMembers.Num() - 1, Ring, IndexInRing);
		DistanceToCheck = (Ring * DistancePerRing);
	}

	FVector RayCastMatrix[8]
	{
		FVector(1,0,0),
		FVector(1,1,0),
		FVector(0,1,0),
		FVector(-1,1,0),
		FVector(-1,0,0),
		FVector(-1,-1,0),
		FVector(0,-1,0),
		FVector(1,-1,0),
	};

	//we need the nav system to check if the new position is a valid moveable position
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	check(NavSystem);
	FNavLocation NavLocation = FNavLocation();
	NavSystem->ProjectPointToNavigation(OriginalLocation, NavLocation, FVector::One() * DistanceToCheck);
	FVector HeightenedPosition = NavLocation.Location + FVector(0.0, 0.0, 250.0);

	FVector TempLocation = FVector::Zero();
	int AddedLocations = 0;

	{
		//We need these temporary values only during the execution of the loop
		FHitResult HitResult;
		FNavLocation TempProjection;

		for (int i = 0; i < 8; i++)
		{
			if (GetWorld()->LineTraceSingleByChannel(HitResult, (RayCastMatrix[i] * DistanceToCheck) + HeightenedPosition, RayCastMatrix[i] * DistanceToCheck + HeightenedPosition - FVector(0, 0, 1750), ECC_Visibility))
			{
#if WITH_EDITOR
				DrawDebugLine(GetWorld(),
					(RayCastMatrix[i] * DistanceToCheck) + HeightenedPosition,
					RayCastMatrix[i] * DistanceToCheck + HeightenedPosition - FVector(0, 0, 1750),
					FColor::Red, false, 5);
#endif

				NavSystem->ProjectPointToNavigation(HitResult.Location, TempProjection, FVector::One() * DistanceToCheck);
				if (TempProjection.Location.X != 0 && TempProjection.Location.Y != 0)
				{
					AddedLocations++;
					TempLocation += TempProjection.Location;
				}
			}
		}
	}
	if (NavLocation.Location != FVector::Zero())
	{
		AddedLocations++;
		TempLocation += NavLocation.Location;
	}


	NavSystem->ProjectPointToNavigation((TempLocation / AddedLocations), NavLocation, FVector::One() * DistanceToCheck);

#if WITH_EDITOR
	if (false)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,
			"Original Location = " + OriginalLocation.ToString() +
			"\n | Pre-Project Location = " + (TempLocation / AddedLocations).ToString() +
			"\n | Projected Location = " + NavLocation.Location.ToString());
	}

#endif

	return (NavLocation.Location.X == 0 && NavLocation.Location.Y == 0) ?
		Leader->GetOwner()->GetActorLocation() : NavLocation.Location;
}

FVector USquadFormation_Circle::GetMoveLocationForMember_Implementation(int MemberIndex, FVector OriginalMoveLocation) const
{
	if (MemberIndex == 0) return OriginalMoveLocation;

	int Ring = 0;
	int IndexPositionInRing = 0;
	GetRing(MemberIndex, Ring, IndexPositionInRing);

	const float Degree = (360.0 / (UnitsPerRing * Ring)) * IndexPositionInRing;
	const float PositionX = (DistancePerRing * Ring) * cos(Degree * PI / 180);
	const float PositionY = (DistancePerRing * Ring) * sin(Degree * PI / 180);

	//Debug prints
#if WITH_EDITOR
	if (false)
	{
		FString printValue =
			"PositionX = " + FString::SanitizeFloat(PositionX, 4) +
			" | PositionY = " + FString::SanitizeFloat(PositionY, 4) +
			" | Ring Number = " + FString::FromInt(Ring) +
			" | Position In Ring = " + FString::FromInt(IndexPositionInRing) +
			" | Degree = " + FString::SanitizeFloat(Degree, 4);

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,
			printValue);
	}
#endif

	//we need the nav system to check if the new position is a valid moveable position
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	check(NavSystem);
	FNavLocation NavLocation = FNavLocation();
	NavSystem->ProjectPointToNavigation(FVector(PositionX + OriginalMoveLocation.X, PositionY + OriginalMoveLocation.Y, OriginalMoveLocation.Z), NavLocation, FVector::One() * DistancePerRing * Ring);

	return NavLocation.Location;
}

void USquadFormation_Circle::GetRing(int MemberIndex, int& Ring, int& IndexPositionInRing) const
{
	int currentRingNumber = 0;
	int lastLoopIndex = 0;
	for (int i = 1; i < 100; i += i * 2) //if this actually reaches 100 you did something wrong
	{
		currentRingNumber++;
		if ((static_cast<float>(MemberIndex)) / (UnitsPerRing * i) <= 1)
		{
			Ring = currentRingNumber;
			IndexPositionInRing = MemberIndex - UnitsPerRing * lastLoopIndex;
			break;
		}
		lastLoopIndex = i;
	}
}


