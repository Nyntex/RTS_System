// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/MouseHover_Component.h"

#include "Evaluation/PreAnimatedState/MovieScenePreAnimatedCaptureSources.h"
#include "Interfaces/ISelectable.h"
#include "Squad/ISquad.h"

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

void UMouseHover_Component::HoverSquad(AActor* ActorOfSquad)
{
	if (!ActorOfSquad) return;
	if (CurrentHover == ActorOfSquad) return;

	//stop the current hover
	if(CurrentHover)
	{
		if (IISquad::Execute_HasSquad(CurrentHover))
		{
			TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(CurrentHover);
			for(AActor* SquadMember : SquadActors)
			{
				if (!SquadMember) continue;
				IISelectable::Execute_StopHover(SquadMember);
			}
		}
		else
		{
			IISelectable::Execute_StopHover(CurrentHover);
		}
	}

	CurrentHover = ActorOfSquad;

	if (IISquad::Execute_HasSquad(CurrentHover))
	{
		TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(CurrentHover);
		for (AActor* SquadMember : SquadActors)
		{
			if (!SquadMember) continue;
			IISelectable::Execute_Hover(SquadMember);
		}
	}
	else
	{
		IISelectable::Execute_Hover(CurrentHover);
	}
}

void UMouseHover_Component::AddActorToHovered(AActor* TargetActor)
{
	if (!TargetActor) return;

	if(!IISquad::Execute_HasSquad(TargetActor))
	{
		IISelectable::Execute_Hover(TargetActor);
		TotalHovered.AddUnique(TargetActor);
		return;
	}

	TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(TargetActor);
	TotalHovered.AddUnique(IISquad::Execute_GetSquadLeaderActor(TargetActor));

	for(AActor* SquadMember : SquadActors)
	{
		if(!SquadMember) continue;
		IISelectable::Execute_Hover(SquadMember);
		TotalHovered.AddUnique(SquadMember);
	}
}

void UMouseHover_Component::RemoveActorFromHovered(AActor* TargetActor)
{
	if (!TargetActor) return;

	if (!IISquad::Execute_HasSquad(TargetActor))
	{
		IISelectable::Execute_StopHover(TargetActor);
		TotalHovered.RemoveSingle(TargetActor);
		return;
	}

	TArray<AActor*> SquadActors = IISquad::Execute_GetSquadMemberActors(TargetActor);
	TotalHovered.RemoveSingle(IISquad::Execute_GetSquadLeaderActor(TargetActor));

	for (AActor* SquadMember : SquadActors)
	{
		if (!SquadMember) continue;
		IISelectable::Execute_Hover(SquadMember);
	}
}

void UMouseHover_Component::AddActorToSelection(AActor* TargetActor)
{
	if (!TargetActor) return;

	if (!IISquad::Execute_HasSquad(TargetActor))
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

	if (!IISquad::Execute_HasSquad(TargetActor))
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

TArray<AActor*> UMouseHover_Component::SingularizeActorArray(TArray<AActor*> ToSingularize)
{
	TArray<AActor*> SingularizedActors = TArray<AActor*>();
	for (AActor* target : ToSingularize)
	{
		if (!target->GetClass()->ImplementsInterface(UISelectable::StaticClass())) continue;

		if (IISquad::Execute_HasSquad(target))
		{
			SingularizedActors.AddUnique(IISquad::Execute_GetSquadLeaderActor(target));
		}
		else
		{
			SingularizedActors.AddUnique(target);
		}
	}

	return SingularizedActors;
}

void UMouseHover_Component::MakeNewSelection(TArray<AActor*> ActorsToSelect)
{
	TArray<AActor*> NewlyAddedActors = TArray<AActor*>();
	TArray<AActor*> ActorsToRemove = TotalSelection;
	TArray<AActor*> SingularizedActorsToSelect = SingularizeActorArray(ActorsToSelect);

	for(AActor* target : SingularizedActorsToSelect)
	{
		if (TotalSelection.Contains(target))
		{
			ActorsToRemove.Remove(target);
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

