// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadComponent.h"

// Sets default values for this component's properties
USquadComponent::USquadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	SetIsReplicatedByDefault(true);
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

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, "Server Add Squad Member: " + Actor->GetName());

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

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, "Server Remove Squad Member: " + Actor->GetName());

	Server_RemoveSquadMember(Actor);
}

void USquadComponent::Server_RemoveSquadMember_Implementation(AActor* Actor)
{
	if (!Actor) return;

	if (this != SquadLeader)
	{
		SquadLeader->RemoveSquadMember(Actor);
		return;
	}

	USquadComponent* tempTargetComponent = Actor->FindComponentByClass<USquadComponent>();

	if (!tempTargetComponent)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, Actor->GetName() + " to remove has no Squad Component, are you removing the correct one?");
		return;
	}

	if (Actor == GetOwner()) // is Squad Leader the one to remove
	{
		if (SquadMembers.Num() > 1) // Squad Leader is also part of the Squad Members so if there is one more 
		{
			//Change Squad Leader and make him take over
			//the removal of the squad member with default behavior
			int index = 0;
			if (SquadMembers[0] != this) index = 0;
			else index = 1;

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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, "Number of Members: " + FString::FromInt(SquadMembers.Num()));

	for(USquadComponent* member : SquadMembers)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,  member == this ? "This is me :)" : member->GetOwner()->GetName());
	}
}

void USquadComponent::PrintLeader()
{
	if(!SquadLeader) return;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, SquadLeader == this ? "This is me :)" : SquadLeader->GetOwner()->GetName());
}
