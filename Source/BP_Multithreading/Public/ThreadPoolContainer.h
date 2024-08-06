// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThreadPoolContainer.generated.h"

UENUM(BlueprintType)
enum EBPThreadPriority
{
	Normal,
	AboveNormal,
	BelowNormal,
	Highest,
	Lowest,
	SlightlyBelowNormal,
	TimeCritical,
	Num
};

UCLASS(BlueprintType, Blueprintable)
class UPoolQueueWork : public UObject, public IQueuedWork
{
	GENERATED_BODY()

public:
	virtual void DoThreadedWork() override;
	
	virtual void Abandon() override;

	/**
	 * This is where the real thread work is done. All work that is done for
	 * this queued object should be done from within the call to this function.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDoThreadedWork();

	/**
	* Calling in Game thread
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnPostWork();

	/**
	 * Tells the queued work that it is being abandoned so that it can do
	 * per object clean up as needed. This will only be called if it is being
	 * abandoned before completion. NOTE: This requires the object to delete
	 * itself using whatever heap it was allocated in.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndWork();
};

UCLASS(BlueprintType)
class BP_MULTITHREADING_API UThreadPoolContainer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ThreadPool")
	void CreateThreadPool(const int ThreadsAmount, EBPThreadPriority Priority, FString ThreadPoolName);

	UFUNCTION(BlueprintCallable, Category="ThreadPool")
	void AddWork(UPoolQueueWork* Work);

	UFUNCTION(BlueprintCallable, Category="ThreadPool")
	bool RemoveQueueWork(UPoolQueueWork* Work, const bool ForceClear = false);

private:
	FQueuedThreadPool* ThreadPool;
};
