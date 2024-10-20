// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadComponent.generated.h"

class USquadFormation;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Squad_Component"), Blueprintable )
class RTS_SYSTEM_API USquadComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USquadComponent();

	UPROPERTY(BlueprintReadWrite, Category = "Squad", Replicated)
	TArray<USquadComponent*> SquadMembers;

	UPROPERTY(BlueprintReadWrite, Category = "Squad", Replicated)
	TObjectPtr<USquadComponent> SquadLeader;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squad")
	TSubclassOf<USquadFormation> FormationClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squad", meta = (ShowInnerProperties))
	TObjectPtr<USquadFormation> SquadFormation = nullptr;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//Replicated way to add new squad members. Squad members are required to have
	//the USquadComponent in order to be added to the squad.
	//
	//Works only on owning clients!
	UFUNCTION(BlueprintCallable, Category = "SquadComponent")
	void AddSquadMember(UPARAM(meta=(MustImplement="ISquad")) AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_AddSquadMember(AActor* Actor);

	//Replicated way to remove squad members. Handles the switch of squad leader in the case
	//of the removal of the squad leader.
	//
	//Works only on owning clients!
	UFUNCTION(BlueprintCallable, Category = "SquadComponent")
	void RemoveSquadMember(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_RemoveSquadMember(AActor* Actor);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SquadComponent", meta = (DevelopmentOnly))
	void PrintAllMembers();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SquadComponent", meta=(DevelopmentOnly))
	void PrintLeader();

	UFUNCTION(BlueprintCallable, Category = "SquadComponent")
	FVector GetMoveLocationForMember(int MemberIndex, FVector OriginalMoveLocation) const;

	UFUNCTION(BlueprintCallable, Category = "SquadComponent")
	FVector EvaluateLeaderPosition(FVector OriginalLocation, USquadComponent* Leader) const;
};
