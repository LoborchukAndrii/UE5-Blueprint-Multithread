// Copyright Andrii Loborchuk. All Rights Reserved. (2024)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultithreadingBlueprintLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogThread, Log, All);

DECLARE_DYNAMIC_DELEGATE(FFunctionThreadLogic);

DECLARE_DYNAMIC_DELEGATE_OneParam(FParallelForLogic, int, Index);

UENUM(BlueprintType)
enum EParallelFlags
{
	// Default behavior
	None,

	//Mostly used for testing, when used, ParallelFor will run single threaded instead.
	ForceSingleThread = 1,

	//Offers better work distribution among threads at the cost of a little bit more synchronization.
	//This should be used for tasks with highly variable computational time.
	Unbalanced = 2,

	// if running on the rendering thread, make sure the ProcessThread is called when idle
	PumpRenderingThread = 4,

	// tasks should run on background priority threads
	BackgroundPriority = 8,
};

/**
 * 
 */
UCLASS()
class BP_MULTITHREADING_API UMultithreadingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(DevelopmentOnly), Category = "Threaded")
	static void SleepThread(float SleepTime);

	UFUNCTION(BlueprintCallable, meta=(DevelopmentOnly), Category = "Threaded")
	static void GetThread();

	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void EnableActorMultiThreadTick(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void RunTask_OnBackgroundThread(FFunctionThreadLogic BackGroundThreadLogic, FFunctionThreadLogic GameThreadLogic);

	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void RunTask_OnGameThread(FFunctionThreadLogic GameThreadLogic);

	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void Run_ParallelFor(FParallelForLogic ParallelForLogic, int LoopAmount, EParallelFlags ParallelType = BackgroundPriority);
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThreadLogic);

UCLASS()
class BP_MULTITHREADING_API UAsyncThread : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnBackgroundThread;

	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnGameThread;
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Threaded")
	static UAsyncThread* Threaded_Logic(const UObject* WorldContextObject);

	
private:
	UPROPERTY()
	UObject* WorldContextObject;
};

UCLASS()
class BP_MULTITHREADING_API UAsyncGameThread : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnGameThread;
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Threaded")
	static UAsyncGameThread* GameThread_Logic(const UObject* WorldContextObject);

	
private:
	UPROPERTY()
	UObject* WorldContextObject;
};
