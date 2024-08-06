// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "AtomicContainer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BP_MULTITHREADING_API UAtomicContainer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	void SetAtomic(FName Identifier, UProperty* Value);

	UFUNCTION(BlueprintPure, meta=(CustomStructureParam = "Value"), Category = "Atomics|Get")
	bool GetAtomic(FName Identifier, UProperty*& Value);

	UFUNCTION(BlueprintPure, meta=(CustomStructureParam = "Value"), Category = "Atomics|Get")
	bool GetAtomicAndRemove(FName Identifier, UProperty*& Value);
	
	UFUNCTION(BlueprintCallable, Category="Atomics|Remove")
	bool RemoveAtomic(FName Identifier);

	UFUNCTION(BlueprintCallable, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	void AddQueue(FName Identifier, UProperty* Value);

	UFUNCTION(BlueprintPure, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	bool GetQueue(FName Identifier, UProperty*& Value);

	UFUNCTION(BlueprintCallable, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	void RemoveQueue(FName Identifier);
	
	virtual void BeginDestroy() override;

private:
	TMap<FName, std::atomic<UProperty*>*> AtomicMap;
	TMap<FName, TQueue<UProperty*>*> QueueMap;
};
