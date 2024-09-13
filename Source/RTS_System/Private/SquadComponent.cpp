// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadComponent.h"

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

#endif

	for(USquadComponent* member : SquadMembers)
	{

#if WITH_EDITOR

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,  
			member == this ? "This is me :)" : member->GetOwner()->GetName());

#endif

	}
}

void USquadComponent::PrintLeader()
{
	if(!SquadLeader) return;
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, 
		SquadLeader == this ? "This is me :)" : SquadLeader->GetOwner()->GetName());
#endif
}

#include "Components/SceneComponent.h"
#include "EngineUtils.h"

FVector USquadComponent::GetMoveLocationForMember(int MemberIndex, FVector OriginalMoveLocation, float DistancePerRing, int UnitsPerRing)
{
	if (!SquadLeader) return FVector::Zero();
	if (SquadMembers.IsEmpty()) return FVector::Zero();
	if (MemberIndex == 0) return OriginalMoveLocation;

	int Ring = 0;
	int IndexPositionInRing = 0;
	int j = 0;
	int k = 0;
	for(int i = 1; i < 100; i+=i*2) //if this actually reaches 100 you did something wrong
	{
		j++;
		if((static_cast<float>(MemberIndex)) / (UnitsPerRing * i) <= 1)
		{
			Ring = j;
			IndexPositionInRing = MemberIndex - UnitsPerRing * k;
			break;
		}
		k = i;
	}
	
	float Degree = (360.0 / (UnitsPerRing * Ring)) * IndexPositionInRing;
	float PositionX = (DistancePerRing * Ring) * cos(Degree * PI / 180);
	float PositionY = (DistancePerRing * Ring) * sin(Degree * PI / 180);

#if WITH_EDITOR
	FString printValue = 
		"PositionX = " + FString::SanitizeFloat(PositionX,4) + 
		" | PositionY = " + FString::SanitizeFloat(PositionY,4) + 
		" | Ring Number = " + FString::FromInt(Ring) + 
		" | Position In Ring = " + FString::FromInt(IndexPositionInRing) + 
		" | Degree = " + FString::SanitizeFloat(Degree, 4);
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, 
		printValue);
#endif

	//This is cool Marquee Selection Stuff
	//USceneComponent* obj = CreateDefaultSubobject<USceneComponent>("Name");
	//obj->GetLocalBounds().GetBox();

	//TActorIterator<USceneComponent> itr(GetWorld(), TSubclassOf<USceneComponent>());


	//UNavigationSystemV1::GetNavigationSystem(this);
	return FVector(PositionX + OriginalMoveLocation.X, PositionY + OriginalMoveLocation.Y, OriginalMoveLocation.Z);
}
