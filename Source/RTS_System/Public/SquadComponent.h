// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Squad_Component") )
class RTS_SYSTEM_API USquadComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USquadComponent();

	UPROPERTY(BlueprintReadWrite, Category = "Squad")
	TArray<USquadComponent*> SquadMembers;

	UPROPERTY(BlueprintReadWrite, Category = "Squad")
	USquadComponent* SquadLeader;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Replicated way to add new squad members. Squad members are required to have
	//the USquadComponent in order to be added to the squad.
	//
	//Works only on owning clients!
	UFUNCTION(BlueprintCallable)
	void AddSquadMember(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_AddSquadMember(AActor* Actor);

	//Replicated way to remove squad members. Handles the switch of squad leader in the case
	//of the removal of the squad leader.
	//
	//Works only on owning clients!
	UFUNCTION(BlueprintCallable)
	void RemoveSquadMember(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_RemoveSquadMember(AActor* Actor);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void PrintAllMembers();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void PrintLeader();
};
