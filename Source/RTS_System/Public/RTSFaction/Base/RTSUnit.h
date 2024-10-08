// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "GameFramework/Character.h"
#include "Interfaces/IInstructions.h"
#include "Interfaces/ISelectable.h"
#include "Squad/ISquad.h"
#include "Squad/SquadComponent.h"
#include "RTSUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKilledSignature, ARTSUnit*, SelfActor);

UCLASS(Blueprintable)
class RTS_SYSTEM_API ARTSUnit : public ACharacter,
public IISquad, public IISelectable, public IIInstructions
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARTSUnit();

	UPROPERTY(BlueprintAssignable)
	FKilledSignature OnKilled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Unit", Replicated)
	TObjectPtr<USquadComponent> SquadComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Unit", meta=(ExposeOnSpawn), Replicated)
	TObjectPtr<APlayerState> OwningPlayerState = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Unit|State", Replicated)
	FGameplayTagContainer GameplayTags;

	UPROPERTY(BlueprintReadWrite, Category = "Unit|State")
	FVector LastMoveLocation = FVector::Zero();

private:
	bool MoveInstructedLastTick = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="Unit|Hovering")
	void SetHoverMaterialDepthNumberByRelation(const APlayerState* PlayerToCheck);

	UFUNCTION(BlueprintCallable, Category="Unit|Hovering")
	void SetRenderCustomDepthBySelectionTags();

#pragma region ISelectable
	//UFUNCTION(BlueprintCallable, Category="ISelectable")
	virtual void Hover_Implementation() override;

	//UFUNCTION(BlueprintCallable, Category="ISelectable")
	virtual void StopHover_Implementation() override;

	//UFUNCTION(BlueprintCallable, Category="ISelectable")
	virtual void Select_Implementation() override;

	//UFUNCTION(BlueprintCallable, Category="ISelectable")
	virtual void Deselect_Implementation() override;

#pragma endregion

#pragma region IInstructions
	//This function may only be called on owning clients because it is automatically replicated
	virtual void LocationInstruction_Implementation(const APlayerState* InstructingPlayer, FVector Location) override;

	UFUNCTION(Server, Reliable)
	void Server_LocationInstruction(FVector Location);

#pragma endregion

#pragma region ISquad
	virtual USquadComponent* GetSquadLeader_Implementation() override;

	virtual AActor* GetSquadLeaderActor_Implementation() override;

	virtual bool IsSquadLeader_Implementation() override;

	virtual TArray<USquadComponent*> GetSquadMembers_Implementation() override;

	virtual TArray<AActor*> GetSquadMemberActors_Implementation() override;

	virtual bool HasSquad_Implementation() override;

#pragma endregion
};
