// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UObject/Interface.h"
#include "IInstructions.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIInstructions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RTS_SYSTEM_API IIInstructions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	void LocationInstruction(APlayerState* InstructingPlayer, FVector Location);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	void InteractionInstruction(APlayerState* InstructingPlayer, AActor* ToInteract);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	void RepairInstruction(APlayerState* InstructingPlayer, AActor* ToRepair);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	void ConstructInstruction(APlayerState* InstructingPlayer, AActor* ToConstruct);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	bool CanConstruct();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	bool CanBeConstructed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	bool CanRepair();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	bool CanBeRepaired();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInstructions")
	bool CanBeInteracted();
};
