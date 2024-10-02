// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_System/Public/UtilityFunctions.h"

#include "Components/SceneComponent.h"
#include "EngineUtils.h"
#include "Interfaces/IInstructions.h"

#include "Interfaces/ISelectable.h"
#include "Kismet/GameplayStatics.h"
#include "Squad/ISquad.h"

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

FVector UUtilityFunctions::LineTraceFromMouseToWorld(bool& Hit, FHitResult& OutHit, AActor*& HitActor, UObject* WorldContextObject, float MaxDistance)
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

void UUtilityFunctions::GetMarqueeSelection(UObject* WorldContextObject, const FVector2D FirstPoint, const FVector2D SecondPoint,
	TArray<AActor*>& OutActors, bool bIncludeNonCollidingComponents, bool bActorMustBeFullyEnclosed)
{
    OutActors.Reset();
    FBox2D SelectionRectangle(ForceInit);

    //This method ensures that an appropriate rectangle is generated, 
    //		no matter what the coordinates of first and second point actually are.
    SelectionRectangle += FirstPoint;
    SelectionRectangle += SecondPoint;

    //The Actor Bounds Point Mapping
    const FVector BoundsPointMapping[8] =
    {
        FVector(1.f, 1.f, 1.f),
        FVector(1.f, 1.f, -1.f),
        FVector(1.f, -1.f, 1.f),
        FVector(1.f, -1.f, -1.f),
        FVector(-1.f, 1.f, 1.f),
        FVector(-1.f, 1.f, -1.f),
        FVector(-1.f, -1.f, 1.f),
        FVector(-1.f, -1.f, -1.f)
    };

    USceneComponent* obj = NewObject<USceneComponent>();
    obj->GetLocalBounds().GetBox();

    
    /*for(TActorIterator<USceneComponent> itr(WorldContextObject->GetWorld(), TSubclassOf<USceneComponent>()); itr; ++itr)
    {
	    
    }*/
}

void UUtilityFunctions::GetHighestPrioritySelection(TArray<AActor*> CurrentSelection, TArray<AActor*>& NewSelection)
{
    NewSelection.Empty();
    NewSelection = TArray<AActor*>();
    int LocalCurrentSelectionPriority = -1;

    //Make selection based of priority
    for (AActor* target : CurrentSelection)
    {
        //if the target is no selectable we won't consider it
        if (!target->GetClass()->ImplementsInterface(UIInstructions::StaticClass())) continue;
        
        const int TargetPriority = IISelectable::Execute_GetSelectionPriority(target);
        if(TargetPriority > LocalCurrentSelectionPriority)
        {
            LocalCurrentSelectionPriority = TargetPriority;
            NewSelection.Empty(); //Empty the array so we only have the highest priority in here
            NewSelection.Add(target);
        }
        else
        {
            //When the priority is -1 we only want to have the first selected
            if(LocalCurrentSelectionPriority == -1 && NewSelection.IsEmpty())
            {
                NewSelection.Add(target);
	            
            }
            //otherwise we will add them to the selected list as long as they match the current priority
	        else if(TargetPriority == LocalCurrentSelectionPriority)
	        {
                NewSelection.AddUnique(target);
	        }
        }
    }
}

void UUtilityFunctions::ConvertToSingleSelectables(TArray<AActor*> OriginalSelection, TArray<AActor*>& NewSelection)
{
    NewSelection.Empty();
    NewSelection = TArray<AActor*>();

    for(AActor* target : OriginalSelection)
    {
        //we filter the squad leaders out of the array so we may call 
        if (target->GetClass()->ImplementsInterface(UISquad::StaticClass()))
        {
            if(IISquad::Execute_HasSquad(target))
            {
                NewSelection.AddUnique(IISquad::Execute_GetSquadLeaderActor(target));
            }
        }
        else
        {
			NewSelection.AddUnique(target);
        }
    }
}
