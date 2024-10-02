// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/MouseHover_Component.h"
#include "Interfaces/ISelectable.h"
#include "Squad/ISquad.h"

#define IMPLEMENTSINTERFACE(x) GetClass()->ImplementsInterface(x::StaticClass())

// Sets default values for this component's properties
UMouseHover_Component::UMouseHover_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMouseHover_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMouseHover_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMouseHover_Component::AddActorToHovered(AActor* TargetActor)
{
	if (!TargetActor) return;
	if (!TargetActor->IMPLEMENTSINTERFACE(UISelectable)) return;

	if(!TargetActor->IMPLEMENTSINTERFACE(UISquad))
	{
		IISelectable::Execute_Hover(TargetActor);
		TotalHovered.AddUnique(TargetActor);
		return;
	}

	TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(TargetActor);
	TotalHovered.AddUnique(IISquad::Execute_GetSquadLeaderActor(TargetActor));

	for(AActor* SquadMember : SquadActors)
	{
		if (!SquadMember) continue;
		if (!SquadMember->IMPLEMENTSINTERFACE(UISelectable)) continue;
		IISelectable::Execute_Hover(SquadMember);
	}
}

void UMouseHover_Component::RemoveActorFromHovered(AActor* TargetActor)
{
	if (!TargetActor) return;
	if (!TargetActor->IMPLEMENTSINTERFACE(UISelectable))
	{
		TotalHovered.RemoveSingle(TargetActor);
		return;
	}

	if (!TargetActor->IMPLEMENTSINTERFACE(UISquad))
	{
		IISelectable::Execute_StopHover(TargetActor);
		TotalHovered.RemoveSingle(TargetActor);
		return;
	}

	const TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(TargetActor);
	TotalHovered.RemoveSingle(IISquad::Execute_GetSquadLeaderActor(TargetActor));

	for (AActor* SquadMember : SquadActors)
	{
		if (!SquadMember) continue;
		if (!SquadMember->IMPLEMENTSINTERFACE(UISelectable))
		IISelectable::Execute_StopHover(SquadMember);
	}
}

void UMouseHover_Component::AddActorToSelection(AActor* TargetActor)
{
	if (!TargetActor) return;
	if (!TargetActor->IMPLEMENTSINTERFACE(UISelectable)) return;

	if (!TargetActor->IMPLEMENTSINTERFACE(UISquad))
	{
		IISelectable::Execute_Select(TargetActor);
		TotalSelection.AddUnique(TargetActor);
		return;
		
	}

	TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(TargetActor);
	TotalSelection.Add(IISquad::Execute_GetSquadLeaderActor(TargetActor));

	for (AActor* SquadMember : SquadActors)
	{
		if (!SquadMember) continue;
		IISelectable::Execute_Select(SquadMember);
	}
}

void UMouseHover_Component::RemoveActorFromSelection(AActor* TargetActor)
{
	if (!TargetActor) return;
	if (!TargetActor->IMPLEMENTSINTERFACE(UISelectable))
	{
		TotalSelection.RemoveSingle(TargetActor);
		return;
	}

	if (!TargetActor->IMPLEMENTSINTERFACE(UISquad))
	{
		IISelectable::Execute_Deselect(TargetActor);
		TotalSelection.RemoveSingle(TargetActor);
		return;
		
	}

	TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(TargetActor);
	TotalSelection.RemoveSingle(IISquad::Execute_GetSquadLeaderActor(TargetActor));

	for (AActor* SquadMember : SquadActors)
	{
		if (!SquadMember) continue;
		IISelectable::Execute_Deselect(SquadMember);
	}
}

TArray<AActor*> UMouseHover_Component::SingularizeActorArray(const TArray<AActor*>& ToSingularize)
{
	TArray<AActor*> SingularizedActors = TArray<AActor*>();
	for (AActor* target : ToSingularize)
	{
		if (!target) continue;
		if (!target->IMPLEMENTSINTERFACE(UISelectable)) continue;

		if(target->IMPLEMENTSINTERFACE(UISquad))
		{
			if (IISquad::Execute_HasSquad(target))
			{
				SingularizedActors.AddUnique(IISquad::Execute_GetSquadLeaderActor(target));
			}
		}
		else
		{
			SingularizedActors.AddUnique(target);
		}
	}

	return SingularizedActors;
}

void UMouseHover_Component::MakeNewSelection(const TArray<AActor*>& ActorsToSelect)
{
	TArray<AActor*> NewlyAddedActors = TArray<AActor*>();
	TArray<AActor*> ActorsToRemove = TotalSelection;
	const TArray<AActor*> SingularizedActorsToSelect = SingularizeActorArray(ActorsToSelect);
	NewlyAddedActors.Reserve(SingularizedActorsToSelect.Num());

	for(AActor* target : SingularizedActorsToSelect)
	{
		if (TotalSelection.Contains(target))
		{
			ActorsToRemove.RemoveSingle(target);
			continue;
		}

		NewlyAddedActors.Add(target);
	}

	for(AActor* Removable : ActorsToRemove)
	{
		RemoveActorFromSelection(Removable);
	}

	for(AActor* NewSelected : NewlyAddedActors)
	{
		AddActorToSelection(NewSelected);
	}
}

void UMouseHover_Component::MakeNewHovered(const TArray<AActor*>& ActorsToHover)
{
	TArray<AActor*> NewlyAddedActors = TArray<AActor*>();
	TArray<AActor*> ActorsToRemove = TotalHovered;
	TArray<AActor*> SingularizedActorsToHover = SingularizeActorArray(ActorsToHover);
	NewlyAddedActors.Reserve(SingularizedActorsToHover.Num());

	for (AActor* target : SingularizedActorsToHover)
	{
		if (TotalHovered.Contains(target))
		{
			ActorsToRemove.RemoveSingle(target);
			continue;
		}

		NewlyAddedActors.Add(target);
	}

	for (AActor* Removable : ActorsToRemove)
	{
		RemoveActorFromHovered(Removable);
	}

	for (AActor* NewSelected : NewlyAddedActors)
	{
		AddActorToHovered(NewSelected);
	}
}

