// Copyright 2018 Isaac Hsu.

#pragma once

#include "InputHistoryRecordArray.generated.h"

/**
* A Blueprint-friendly version of InputBufferRecord
**/
USTRUCT(BlueprintType)
struct FInputHistoryRecord
{
	GENERATED_BODY()

	FInputHistoryRecord()
		: bValid(true)
		, StartTime(0.f)
		, EndTime(0.f)
	{}

	FInputHistoryRecord(float InStartTime, float InEndTime, bool bInValid = true)
		: bValid(bInValid)
		, StartTime(InStartTime)
		, EndTime(InEndTime)
	{}

	FInputHistoryRecord(float InTime, bool bInValid = true)
		: bValid(bInValid)
		, StartTime(InTime)
		, EndTime(InTime)
	{}

	/** Whether this record is valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	uint32 bValid : 1;

	/** Time when we start to record it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	float StartTime;

	/** Time when we stop recording it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	float EndTime;

	/** Input events list. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	TArray<FName> Events;

	/** Input events that are translated from. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	TArray<FName> TranslatedEvents;

	FORCEINLINE bool operator == (const FInputHistoryRecord& RHS) const
	{
		return bValid == RHS.bValid 
			&& StartTime == RHS.StartTime 
			&& EndTime == RHS.EndTime 
			&& Events == RHS.Events 
			&& TranslatedEvents == RHS.TranslatedEvents;
	}

	FORCEINLINE bool operator != (const FInputHistoryRecord& RHS) const
	{
		return !this->operator==(RHS);
	}
};

/**
* An object containing an array of InputHistoryRecord.
**/
UCLASS(BlueprintType, ClassGroup = (Input))
class INPUTBUFFER_API UInputHistoryRecordArray : public UObject
{
	GENERATED_BODY()

public:

	/* An array of InputHistoryRecord. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	TArray<FInputHistoryRecord> Records;
};

