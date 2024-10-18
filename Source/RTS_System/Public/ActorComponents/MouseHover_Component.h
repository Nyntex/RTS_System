// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseHover_Component.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTS_SYSTEM_API UMouseHover_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMouseHover_Component();
	UPROPERTY(BlueprintReadWrite, Category="MouseHoverComponent")
	AActor* CurrentHover = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="MouseHoverComponent")
	AActor* CurrentSelected = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="MouseHoverComponent")
	TArray<AActor*> TotalHovered = TArray<AActor*>();

	//Contains all actors that are currently selected
	//For Squads -> will only contain the squad leader to reduce the size of the array
	//For Squads -> when on member is selected only it's leader will be added to the array
	//and all others will only have the related interface function called
	UPROPERTY(BlueprintReadWrite, Category="MouseHoverComponent")
	TArray<AActor*> TotalSelection = TArray<AActor*>();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "TargetActor"))
	void AddActorToHovered(AActor* TargetActor);
	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "TargetActor"))
	void RemoveActorFromHovered(AActor* TargetActor);

	//Adds the chosen Actor to the selection as well as calling the ISelection interface Select
	//method on that target. If the target has a squad only the squad leader will be added to
	//the selection but the Select method will be called on all members of that squad
	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "TargetActor"))
	void AddActorToSelection(AActor* TargetActor);
	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "TargetActor"))
	void RemoveActorFromSelection(AActor* TargetActor);

	//removes unnecessary actors from the array, like squad members that are not the leader
	//if it has a squad.
	//This also removes any actor that may not be selectable
	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "ToSingularize"))
	TArray<AActor*> SingularizeActorArray(const TArray<AActor*>& ToSingularize);

	//This function is really performance heavy because there are many loops
	//Actors that are currently selected but not in the array will be deselected
	//Actors that are currently selected and in the array won't change
	//Actors that are not currently selected but are in the array will be selected
	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "ActorsToSelect"))
	void MakeNewSelection(const TArray<AActor*>& ActorsToSelect);

	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (AutoCreateRefTerm = "ActorsToHover"))
	void MakeNewHovered(const TArray<AActor*>& ActorsToHover);

	UFUNCTION(BlueprintCallable, Category = "MouseHoverComponent", meta = (ReturnDisplayName = "PrioritizedArray"))
	TArray<AActor*> ApplySelectionPriority(const TArray<AActor*>& SingularizedActors);
};