// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_System/Public/UtilityFunctions.h"

#include "Kismet/GameplayStatics.h"

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

FVector UUtilityFunctions::LineTraceFromMouseToWorld(UObject* WorldContextObject, float MaxDistance, AActor*& HitActor, bool& Hit, FHitResult& OutHit)
{
    //Variable Setup
    UWorld* WorldContext = WorldContextObject->GetWorld();

    FVector WorldDirection = FVector();
    FVector ignored = FVector();
    WorldContext->GetFirstPlayerController()->DeprojectMousePositionToWorld(ignored, WorldDirection);

    FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(WorldContext, 0)->GetCameraLocation();
    FVector EndLocation = CameraLocation + (WorldDirection * MaxDistance);

    //LineTrace
    Hit = WorldContext->LineTraceSingleByChannel(OutHit, CameraLocation, EndLocation, ECC_Visibility);

    HitActor = OutHit.GetActor();
	return OutHit.Location;
}
