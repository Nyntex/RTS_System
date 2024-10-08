// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/StatsComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (StatsChanged) CalculateNewStats();
}

void UStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStatsComponent, CalculatedStats);
	DOREPLIFETIME(UStatsComponent, BaseAdditiveStats);
	DOREPLIFETIME(UStatsComponent, AdditiveStats);
	DOREPLIFETIME(UStatsComponent, MultiplierStats);
}

void UStatsComponent::CalculateNewStats()
{
	StatsChanged = false;
}

void UStatsComponent::AddStat(TArray<FGameplayStat>& StatContainer, const FGameplayTag& Tag, const float& Amount)
{
	StatsChanged = true;
	if (int index = StatContainer.Find(FGameplayStat(Tag)))
	{
		StatContainer[index].Add(Amount);
	}
	StatContainer.Add(FGameplayStat(Tag, Amount));
}

void UStatsComponent::RemoveStat(TArray<FGameplayStat>& StatContainer, const FGameplayTag& Tag,
	const float& Amount)
{
	StatsChanged = true;
	if (int index = StatContainer.Find(Tag))
	{
		StatContainer[index].Remove(Amount);
		if (StatContainer[index] <= 0.0f)
		{
			StatContainer.Remove(Tag);
		}
	}
}

void UStatsComponent::AddMultiplierStat(const FGameplayTag& Tag, const float& Amount)
{
	AddStat(MultiplierStats, Tag, Amount);
}

void UStatsComponent::RemoveMultiplierStat(const FGameplayTag& Tag, const float& Amount)
{
	RemoveStat(MultiplierStats, Tag, Amount);
}

void UStatsComponent::AddBaseAdditiveStat(const FGameplayTag& Tag, const float& Amount)
{
	AddStat(BaseAdditiveStats, Tag, Amount);
}

void UStatsComponent::RemoveBaseAdditiveStat(const FGameplayTag& Tag, const float& Amount)
{
	RemoveStat(BaseAdditiveStats, Tag, Amount);
}

void UStatsComponent::AddAdditiveStat(const FGameplayTag& Tag, const float& Amount)
{
	AddStat(AdditiveStats, Tag, Amount);
}

void UStatsComponent::RemoveAdditiveStat(const FGameplayTag& Tag, const float& Amount)
{
	RemoveStat(AdditiveStats, Tag, Amount);
}

