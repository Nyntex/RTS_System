// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

//A stat containing a GameplayTag and a value. Value is clamped to 0 if it goes below 0.
//The stat value cannot be less than 0
USTRUCT(Blueprintable, BlueprintType)
struct FGameplayStat
{
	GENERATED_BODY()

	FGameplayStat() : StatTag(FGameplayTag()), StatValue(0) {};

	FGameplayStat(const FGameplayTag& Tag, const float Value)
		: StatTag(Tag), StatValue(Value) {};

	FGameplayStat(const FGameplayTag& Tag)
		: StatTag(Tag), StatValue(0) {};

	//The Tag may only be used with the correct category of tags.
	//If you want to make any tag available remove the meta=(Category="") specifier
	UPROPERTY(EditAnywhere, meta=(Categories="RTS.Stats"))
	FGameplayTag StatTag;

	//This value cannot go below 0
	UPROPERTY(EditAnywhere, meta=(ClampMin=0))
	float StatValue;


#pragma region Operator
	FORCEINLINE bool operator==(const FGameplayTag& Other) const
	{
		return StatTag.MatchesTagExact(Other);
	}

	FORCEINLINE bool operator==(const FGameplayStat& Other) const
	{
		return StatTag.MatchesTagExact(Other.StatTag);
	}

	FORCEINLINE bool operator==(const float& Other) const
	{
		return StatValue == Other;
	}

	FORCEINLINE bool operator>(const float& Other) const
	{
		return StatValue > Other;
	}

	FORCEINLINE bool operator>=(const float& Other) const
	{
		return StatValue >= Other;
	}

	FORCEINLINE bool operator<(const float& Other) const
	{
		return StatValue < Other;
	}

	FORCEINLINE bool operator<=(const float& Other) const
	{
		return StatValue <= Other;
	}
#pragma endregion

	FORCEINLINE void Add(const float& Amount)
	{
		StatValue += Amount >= 0 ? Amount : (Amount * (-1));
	}

	FORCEINLINE void Multiply(const float& Amount)
	{
		StatValue *= Amount >= 0 ? Amount : Amount * (-1);
	}

	FORCEINLINE void Subtract(const float& Amount)
	{
		StatValue -= Amount >= 0 ? Amount : (Amount * (-1));
		if (StatValue <= 0.0f) StatValue = 0.0f;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "StatsComponent", ScriptName="StatsComponent"), Blueprintable )
class RTS_SYSTEM_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

	//the total calculated stats containing all stats with all
	//bonuses and multipliers applied
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="StatsComponent", Replicated)
	TArray<FGameplayStat> CalculatedStats;

	//The base stats, no bonuses or multipliers added
	//These stats should not be edited in-game but should only be setup once
	//Replication is therefore not required
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatsComponent")
	TArray<FGameplayStat> BaseStats;

	//The stats that get added to the base stats to be multiplied by multipliers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StatsComponent", Replicated)
	TArray<FGameplayStat> BaseAdditiveStats;

	//Stats that get added additively after multiplier calculation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StatsComponent", Replicated)
	TArray<FGameplayStat> AdditiveStats;

	//Multiplier to stats, Multipliers stack additively
	//These Stats work in floating points, meaning: 1,0 = 100%
	//
	//Example: Base HP = 100 with 2 100% HP Multiplier
	//Wrong: 100 * 2 * 2 = 400
	//Correct: 100 * (1+(1+1)) = 300
	UPROPERTY(EditAnywhere, Category="StatsComponent", Replicated)
	TArray<FGameplayStat> MultiplierStats;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool StatsChanged;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Category = "StatsComponent")
	void CalculateNewStats();

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void AddStat(UPARAM(ref) TArray<FGameplayStat>& StatContainer, FGameplayTag Tag, const float& Amount);
	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void RemoveStat(UPARAM(ref) TArray<FGameplayStat>& StatContainer, FGameplayTag Tag, const float& Amount);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void AddMultiplierStat(FGameplayTag Tag, const float& Amount);

	//Lowers the given multiplier stat by that amount. If there is no multiplier
	//with that value it will be ignored.
	//
	//Beware not to add negative numbers or else it will increase the stat if it exists.
	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void RemoveMultiplierStat(FGameplayTag Tag, const float& Amount);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void AddBaseAdditiveStat(FGameplayTag Tag, const float& Amount);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void RemoveBaseAdditiveStat(FGameplayTag Tag, const float& Amount);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void AddAdditiveStat(FGameplayTag Tag, const float& Amount);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void RemoveAdditiveStat(FGameplayTag Tag, const float& Amount);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void HasStatWithTag(FGameplayTag GameplayTag, bool& HasTag);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta = (Categories = "RTS.Stats"))
	void HasStat(FGameplayTag GameplayTag, bool& HasStat, FGameplayStat& OutStat);

	UFUNCTION(BlueprintCallable, Category= "StatsComponent", meta=(ReturnDisplayName="StatsChanged"))
	bool StatsChangedLastTick();
};
