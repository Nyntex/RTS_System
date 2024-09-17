// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadComponent.h"
#include "NavigationSystem.h"



// Sets default values for this component's properties
USquadComponent::USquadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	SquadMembers = TArray<USquadComponent*>();
	SquadLeader = nullptr;

	// ...
}


// Called when the game starts
void USquadComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USquadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USquadComponent::AddSquadMember(AActor* Actor)
{
	if (!Actor) return;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, "Server Add Squad Member: " + Actor->GetName());
#endif

	Server_AddSquadMember(Actor);
}

void USquadComponent::Server_AddSquadMember_Implementation(AActor* Actor)
{
	if (!Actor) return;

	if(!SquadLeader)
	{
		SquadLeader = this;
		SquadMembers = TArray<USquadComponent*>();
		SquadMembers.AddUnique(this);
	}

	if (Actor == SquadLeader->GetOwner() || Actor == GetOwner()) return;

	if (this != SquadLeader)
	{
		SquadLeader->Server_AddSquadMember(Actor);
		return;
	}

	USquadComponent* tempTargetComponent = Actor->FindComponentByClass<USquadComponent>();

	if (!tempTargetComponent)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, Actor->GetName() + " to add has no Squad Component, are you adding the correct one?");
		return;
	}

	SquadMembers.AddUnique(tempTargetComponent);
	for (USquadComponent* member : SquadMembers)
	{
		member->SquadMembers = SquadMembers;
		member->SquadLeader = SquadLeader;
	}
}

void USquadComponent::RemoveSquadMember(AActor* Actor)
{
	if (!Actor) return;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, "Server Remove Squad Member: " + Actor->GetName());
#endif
	Server_RemoveSquadMember(Actor);
}

void USquadComponent::Server_RemoveSquadMember_Implementation(AActor* Actor)
{
	if (!Actor) return;
	if (!SquadLeader) return;
	if (SquadMembers.IsEmpty()) return;

	if (this != SquadLeader)
	{
		SquadLeader->RemoveSquadMember(Actor);
		return;
	}

	USquadComponent* tempTargetComponent = Actor->FindComponentByClass<USquadComponent>();

	if (!tempTargetComponent)
	{
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, Actor->GetName() + " to remove has no Squad Component, are you removing the correct one?");
#endif
		return;
	}

	if (Actor == GetOwner()) // is Squad Leader the one to remove
	{
		if (SquadMembers.Num() > 1) // Squad Leader is also part of the Squad Members so if there is one more 
		{
			//Change Squad Leader and make him take over
			//the removal of the squad member with default behavior
			int index = 0;
			if (SquadMembers[0] != this) index = 1; //This should always be false and therefore never be one

			SquadLeader = SquadMembers[index];

			for (USquadComponent* member : SquadMembers)
			{
				member->SquadLeader = SquadLeader;
			}

			SquadLeader->RemoveSquadMember(Actor);
		}
	}
	else //Remove the target from the list and make it a solo squad
	{
		tempTargetComponent->SquadMembers = TArray<USquadComponent*>();
		tempTargetComponent->SquadLeader = tempTargetComponent;

		SquadMembers.Remove(tempTargetComponent);
		for (USquadComponent* member : SquadMembers)
		{
			member->SquadMembers = SquadMembers;
			member->SquadLeader = SquadLeader;
		}
	}
}

void USquadComponent::PrintAllMembers()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, 
		"Number of Members: " + FString::FromInt(SquadMembers.Num()));

	for(USquadComponent* member : SquadMembers)
	{


		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,  
			member == this ? "This is me :)" : member->GetOwner()->GetName());


	}
#endif
}

void USquadComponent::PrintLeader()
{
#if WITH_EDITOR
	if(!SquadLeader) return;

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, 
		SquadLeader == this ? "This is me :)" : SquadLeader->GetOwner()->GetName());
#endif
}

FVector USquadComponent::GetMoveLocationForMember(int MemberIndex, FVector OriginalMoveLocation, float DistancePerRing, int UnitsPerRing) const
{
	if (MemberIndex == 0) return OriginalMoveLocation;

	int Ring = 0;
	int IndexPositionInRing = 0;
	GetRing(MemberIndex, UnitsPerRing, Ring, IndexPositionInRing);
	
	const float Degree = (360.0 / (UnitsPerRing * Ring)) * IndexPositionInRing;
	const float PositionX = (DistancePerRing * Ring) * cos(Degree * PI / 180);
	const float PositionY = (DistancePerRing * Ring) * sin(Degree * PI / 180);

	//Debug prints
#if WITH_EDITOR
	if(false)
	{
		FString printValue = 
			"PositionX = " + FString::SanitizeFloat(PositionX,4) + 
			" | PositionY = " + FString::SanitizeFloat(PositionY,4) + 
			" | Ring Number = " + FString::FromInt(Ring) + 
			" | Position In Ring = " + FString::FromInt(IndexPositionInRing) + 
			" | Degree = " + FString::SanitizeFloat(Degree, 4);
		
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, 
			printValue);
	}
#endif

	//we need the nav system to check if the new position is a valid moveable position
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetOwner());
	check(NavSystem);
	FNavLocation NavLocation = FNavLocation();
	NavSystem->ProjectPointToNavigation(FVector(PositionX + OriginalMoveLocation.X, PositionY + OriginalMoveLocation.Y, OriginalMoveLocation.Z), NavLocation, FVector::One() * DistancePerRing * Ring);

	return NavLocation.Location;
}

FVector USquadComponent::EvaluateLeaderPosition(FVector OriginalLocation, USquadComponent* Leader, float DistancePerRing, int UnitsPerRing) const
{
	int DistanceToCheck = 0;
	{
		int Ring = 0;
		int IndexInRing = 0;
		GetRing(Leader->SquadMembers.Num(), UnitsPerRing, Ring, IndexInRing);
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
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetOwner());
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
				if(TempProjection.Location.X != 0 && TempProjection.Location.Y != 0)
				{
					AddedLocations++;
					TempLocation += TempProjection.Location;
				}
			}
		}
	}
	if(NavLocation.Location != FVector::Zero())
	{
		AddedLocations++;
		TempLocation += NavLocation.Location;
	}


	NavSystem->ProjectPointToNavigation((TempLocation / AddedLocations), NavLocation, FVector::One() * DistanceToCheck);

#if WITH_EDITOR
	if(false)
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

void USquadComponent::GetRing(int MemberIndex, int UnitsPerRing, int& Ring, int& IndexPositionInRing) const
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
