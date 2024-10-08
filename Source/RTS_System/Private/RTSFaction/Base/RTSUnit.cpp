// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSFaction/Base/RTSUnit.h"

#include "AIController.h"
#include "AI/NavigationSystemBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#define IMPLEMENTSINTERFACE(x) GetClass()->ImplementsInterface(x::StaticClass())

// Sets default values
ARTSUnit::ARTSUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SquadComponent = CreateDefaultSubobject<USquadComponent>(TEXT("Squad Component"));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetAreaClassOverride(FNavigationSystem::GetDefaultObstacleArea());
	GetCapsuleComponent()->SetCanEverAffectNavigation(true);
}

// Called when the game starts or when spawned
void ARTSUnit::BeginPlay()
{
	Super::BeginPlay();

	//this checks the local player controller with the owning player controller in order to set the correct
	//hover material
	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	if(controller->GetPlayerState<APlayerState>())
	{
		SetHoverMaterialDepthNumberByRelation(controller->GetPlayerState<APlayerState>());
	}
}

// Called every frame
void ARTSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(MoveInstructedLastTick) //Make it so the location instruction is only called once per tick 
	{
		MoveInstructedLastTick = false;
	}
}

// Called to bind functionality to input
void ARTSUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARTSUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSUnit, GameplayTags);
	DOREPLIFETIME(ARTSUnit, SquadComponent);
	DOREPLIFETIME(ARTSUnit, OwningPlayerState);
}

void ARTSUnit::SetHoverMaterialDepthNumberByRelation(const APlayerState* PlayerToCheck)
{
	//1 - Owned; 2 - Ally; 3 - Enemy;
	//TODO: CHECK IF PLAYER STATE IS ALLY AND SET THE VALUE ACCORDINGLY 

	if(!PlayerToCheck && OwningPlayerState)
	{
		GetMesh()->SetCustomDepthStencilValue(3);
	}

	if(PlayerToCheck->GetPlayerId() == OwningPlayerState->GetPlayerId())
	{
		GetMesh()->SetCustomDepthStencilValue(1);
	}
	else
	{
		GetMesh()->SetCustomDepthStencilValue(3);
	}
}

void ARTSUnit::SetRenderCustomDepthBySelectionTags()
{
	const FGameplayTag TagToCheck = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("RTS.Selection.Hovering"));
	const FGameplayTag TagToCheck2 = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("RTS.Selection.Selected"));

	if (!TagToCheck.IsValid() || !TagToCheck2.IsValid()) return;

	GetMesh()->SetRenderCustomDepth(GameplayTags.HasTag(TagToCheck) || GameplayTags.HasTag(TagToCheck2));
}

#pragma region ISelectable

void ARTSUnit::Hover_Implementation()
{
	GameplayTags.AddTag(UGameplayTagsManager::Get().RequestGameplayTag(TEXT("RTS.Selection.Hovering")));
#if WITH_ENGINE
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,
		"Hovered Unit");
#endif
	SetRenderCustomDepthBySelectionTags();
}

void ARTSUnit::StopHover_Implementation()
{
	GameplayTags.RemoveTag(UGameplayTagsManager::Get().RequestGameplayTag(TEXT("RTS.Selection.Hovering")));
#if WITH_ENGINE
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red,
		"Stopped Hovered Unit");
#endif
	SetRenderCustomDepthBySelectionTags();
}

void ARTSUnit::Select_Implementation()
{
	GameplayTags.AddTag(UGameplayTagsManager::Get().RequestGameplayTag(TEXT("RTS.Selection.Selected")));
	SetRenderCustomDepthBySelectionTags();
}

void ARTSUnit::Deselect_Implementation()
{
	GameplayTags.RemoveTag(UGameplayTagsManager::Get().RequestGameplayTag(TEXT("RTS.Selection.Selected")));
	SetRenderCustomDepthBySelectionTags();
}
#pragma endregion

#pragma region IInstructions

void ARTSUnit::LocationInstruction_Implementation(const APlayerState* InstructingPlayer, FVector Location)
{
	if (MoveInstructedLastTick) return;
	if (!InstructingPlayer) return;
	if (InstructingPlayer != OwningPlayerState) return;

	if(!IsSquadLeader_Implementation())
	{
		AActor* SquadLeaderActor = GetSquadLeaderActor_Implementation();
		if(SquadLeaderActor)
		{
			if(SquadLeaderActor->IMPLEMENTSINTERFACE(UIInstructions))
			{
				Execute_LocationInstruction(SquadLeaderActor, OwningPlayerState, Location);
			}
		}
	}

	Server_LocationInstruction(Location);
}

void ARTSUnit::Server_LocationInstruction_Implementation(FVector Location)
{
	if (MoveInstructedLastTick) return;
	MoveInstructedLastTick = true;

	LastMoveLocation = SquadComponent->EvaluateLeaderPosition(Location, SquadComponent);

	TArray<AActor*> SquadMembers = GetSquadMemberActors_Implementation();

	for(int i = 0; i < SquadMembers.Num(); ++i)
	{
		const FVector MoveLocationForMember = SquadComponent->GetMoveLocationForMember(i, LastMoveLocation);
		Cast<AAIController>(Cast<APawn>(SquadMembers[i])->GetController())->MoveToLocation(MoveLocationForMember);
	}

}
#pragma endregion

#pragma region ISquad
USquadComponent* ARTSUnit::GetSquadLeader_Implementation()
{
	return SquadComponent->SquadLeader;
}

AActor* ARTSUnit::GetSquadLeaderActor_Implementation()
{
	return SquadComponent->SquadLeader->GetOwner();
}

bool ARTSUnit::IsSquadLeader_Implementation()
{
	return SquadComponent->SquadLeader == SquadComponent;
}

TArray<USquadComponent*> ARTSUnit::GetSquadMembers_Implementation()
{
	return SquadComponent->SquadMembers;
}

TArray<AActor*> ARTSUnit::GetSquadMemberActors_Implementation()
{
	TArray<AActor*> SquadMembers = TArray<AActor*>();
	SquadMembers.Reserve(SquadComponent->SquadMembers.Num());

	for(USquadComponent* SquadMember : SquadComponent->SquadMembers)
	{
		if (!SquadMember) continue;
		SquadMembers.Add(SquadMember->GetOwner());
	}

	return SquadMembers;
}

bool ARTSUnit::HasSquad_Implementation()
{
	return SquadComponent->SquadLeader && SquadComponent->SquadMembers.Num() > 0;
}
#pragma endregion

