// Fill out your copyright notice in the Description page of Project Settings.


#include "Squad/SquadComponent.h"
#include "Squad/SquadFormation.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USquadComponent::USquadComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	SquadMembers = TArray<USquadComponent*>();
	SquadLeader = nullptr;
	if(FormationClass)
	{
		SquadFormation = NewObject<USquadFormation>(this, FormationClass, FormationClass->GetFName());
		//SquadFormation = Cast<USquadFormation>(CreateDefaultSubobject(FormationClass->GetFName(), FormationClass.Get(), FormationClass.Get(), true, false));
		SquadFormation->SquadComponent = this;
	}
}


// Called when the game starts
void USquadComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void USquadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USquadComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USquadComponent, SquadMembers);
	DOREPLIFETIME(USquadComponent, SquadLeader);
}

#if WITH_EDITOR
void USquadComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (GEngine->IsEditor())
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,
			"SquadComponent PostEditChange");
		if (FormationClass != nullptr)
		{
			if (SquadFormation == nullptr)
			{
				SquadFormation = NewObject<USquadFormation>(this, FormationClass, FormationClass->GetFName());
				//SquadFormation = Cast<USquadFormation>(CreateDefaultSubobject(FormationClass->GetFName(), FormationClass.Get(), FormationClass.Get(), true, false));
			}
			else if (SquadFormation->GetClass()->GetFName() != FormationClass->GetFName() && FormationClass->IsValidLowLevel())
			{
				SquadFormation = NewObject<USquadFormation>(this, FormationClass, FormationClass->GetFName());
				//SquadFormation = Cast<USquadFormation>(CreateDefaultSubobject(FormationClass->GetFName(), FormationClass.Get(), FormationClass.Get(), true, false));
			}
			SquadFormation->SquadComponent = this;
		}
		else
		{
			SquadFormation = nullptr;
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
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
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if(!SquadLeader) return;

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, 
		SquadLeader == this ? "This is me :)" : SquadLeader->GetOwner()->GetName());
#endif
}

FVector USquadComponent::GetMoveLocationForMember(int MemberIndex, FVector OriginalMoveLocation) const
{
	return SquadFormation->GetMoveLocationForMember(MemberIndex, OriginalMoveLocation);
}

FVector USquadComponent::EvaluateLeaderPosition(FVector OriginalLocation, USquadComponent* Leader) const
{
	return SquadFormation->EvaluateLeaderPosition(OriginalLocation, Leader);
}
