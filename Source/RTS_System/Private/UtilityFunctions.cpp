// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_System/Public/UtilityFunctions.h"

FVector UUtilityFunctions::ConvertPositionToBuildingPosition(FVector original, float buildingSize)
{
    int xLocation = static_cast<int>((original.X + (original.X > 0 ? (buildingSize / 2) : (-buildingSize / 2))) / buildingSize) * buildingSize;
    int yLocation = static_cast<int>((original.Y + (original.Y > 0 ? (buildingSize / 2) : (-buildingSize / 2))) / buildingSize) * buildingSize;
    FVector retVal = FVector(xLocation, yLocation, original.Z);

#if WITH_EDITOR
    if(GIsEditor)
    GEngine->AddOnScreenDebugMessage(0, 10, FColor::Red, "Original: " + original.ToString() + " \n Computed: " + retVal.ToString());
#endif

    return retVal;
}
