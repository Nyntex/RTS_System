// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Squad/SquadComponent.h"
#include "UObject/Interface.h"
#include "ISquad.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UISquad : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RTS_SYSTEM_API IISquad
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="ISquad")
	TArray<AActor*> GetSquadMemberActors();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISquad")
	AActor* GetSquadLeaderActor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISquad")
	USquadComponent* GetSquadLeader();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISquad")
	TArray<USquadComponent*> GetSquadMembers();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISquad")
	bool HasSquad();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISquad")
	bool IsSquadLeader();
};
