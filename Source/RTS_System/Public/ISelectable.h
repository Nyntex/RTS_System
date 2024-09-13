// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISelectable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UISelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RTS_SYSTEM_API IISelectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Selects the unit showing new stuff in UI (if needed) and showing the outline
	//If this is implemented: Also implement "Deselect"
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISelectable")
	void Select();

	//Removes from selection
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISelectable")
	void Deselect();

	//Hovering over a unit should not permanently show stuff in the UI but show the outline
	//as long as it is hovered. For Units it may also show Unit Information in the UI for the duration.
	//If implemented: Also implement "StopHover"
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISelectable")
	void Hover();

	//Stops the outline and the unit information showcase 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISelectable")
	void StopHover();

	//Should return true when selected, this may either be evaluated with a variable in the object, by checking
	//gameplay tags or the state of the custom depth render.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISelectable")
	bool IsSelected();

	//The selection priority is used to evaluate which units or buildings should be
	//combined in the selection. Buildings should be unique, therefore we will use -1 as the single case with
	//the least priority, having two with priority -1 selected will only have the first selected.
	//This is also used to select all builders in the marquee but stop selecting them when there is a
	//fighting unit inside the marquee.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISelectable")
	int GetSelectionPriority();
};
