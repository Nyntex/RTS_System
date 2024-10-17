// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/StatsComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);

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
	CalculatedStats = BaseStats;

	//add base additive stats
	for(FGameplayStat GameplayStat : BaseAdditiveStats)
	{
		//should return true when the tag is the same since the equality operator has been overriden
		if(int i = CalculatedStats.Find(GameplayStat))
		{
			CalculatedStats[i].Add(GameplayStat.StatValue);
			continue;
		}

		CalculatedStats.AddUnique(GameplayStat);
	}

	//add multipliers
	for (FGameplayStat GameplayStat : MultiplierStats)
	{
		//should return true when the tag is the same since the equality operator has been overriden
		if (int i = CalculatedStats.Find(GameplayStat))
		{
			CalculatedStats[i].Multiply(GameplayStat.StatValue);
		}
	}

	//add non multiplied additive stats
	for (FGameplayStat GameplayStat : AdditiveStats)
	{
		//should return true when the tag is the same since the equality operator has been overriden
		if (int i = CalculatedStats.Find(GameplayStat))
		{
			CalculatedStats[i].Add(GameplayStat.StatValue);
			continue;
		}

		CalculatedStats.AddUnique(GameplayStat);
	}
}

void UStatsComponent::AddStat(TArray<FGameplayStat>& StatContainer, FGameplayTag Tag, const float& Amount)
{
	StatsChanged = true;
	if (int index = StatContainer.Find(FGameplayStat(Tag)))
	{
		StatContainer[index].Add(Amount);
	}
	StatContainer.Add(FGameplayStat(Tag, Amount));
}

void UStatsComponent::RemoveStat(TArray<FGameplayStat>& StatContainer, FGameplayTag Tag, const float& Amount)
{
	StatsChanged = true;
	if (int index = StatContainer.Find(Tag))
	{
		StatContainer[index].Subtract(Amount);
		if (StatContainer[index] <= 0.0f)
		{
			StatContainer.Remove(Tag);
		}
	}
}

void UStatsComponent::AddMultiplierStat(FGameplayTag Tag, const float& Amount)
{
	AddStat(MultiplierStats, Tag, Amount);
}

void UStatsComponent::RemoveMultiplierStat(FGameplayTag Tag, const float& Amount)
{
	RemoveStat(MultiplierStats, Tag, Amount);
}

void UStatsComponent::AddBaseAdditiveStat(FGameplayTag Tag, const float& Amount)
{
	AddStat(BaseAdditiveStats, Tag, Amount);
}

void UStatsComponent::RemoveBaseAdditiveStat(FGameplayTag Tag, const float& Amount)
{
	RemoveStat(BaseAdditiveStats, Tag, Amount);
}

void UStatsComponent::AddAdditiveStat(FGameplayTag Tag, const float& Amount)
{
	AddStat(AdditiveStats, Tag, Amount);
}

void UStatsComponent::RemoveAdditiveStat(FGameplayTag Tag, const float& Amount)
{
	RemoveStat(AdditiveStats, Tag, Amount);
}

void UStatsComponent::HasStatWithTag(FGameplayTag GameplayTag, bool& HasTag)
{
	HasTag = false;

	for (FGameplayStat GameplayStat : CalculatedStats)
	{
		if (GameplayStat.StatTag.MatchesTagExact(GameplayTag))
		{
			HasTag = true;
			return;
		}
	}
}

void UStatsComponent::HasStat(FGameplayTag GameplayTag, bool& HasStat, FGameplayStat& OutStat)
{
	HasStat = false;
	OutStat = FGameplayStat();

	for(FGameplayStat GameplayStat : CalculatedStats)
	{
		if(GameplayStat.StatTag.MatchesTagExact(GameplayTag))
		{
			HasStat = true;
			OutStat = GameplayStat;
			return;
		}
	}
}

bool UStatsComponent::StatsChangedLastTick()
{
	return StatsChanged;
}

