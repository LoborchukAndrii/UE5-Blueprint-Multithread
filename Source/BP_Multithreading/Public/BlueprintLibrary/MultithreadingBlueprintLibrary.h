// Copyright Andrii Loborchuk. All Rights Reserved. (2024)

#pragma once

#include "CoreMinimal.h"
#include "Data/MutexType.h"
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
	/**
	 * This function used only for testing is your code has impact to game thread and create some stutters.
	 * 
	 * @param SleepTime - The time during which the thread will be asleep.
	 */
	UFUNCTION(BlueprintCallable, meta=(DevelopmentOnly, BlueprintThreadSafe), Category = "Multi-Threading | Debug")
	static void SleepThread(float SleepTime);

	/**
	 * Debug function to check on which thread code was executed.
	 */
	UFUNCTION(BlueprintCallable, meta=(DevelopmentOnly, BlueprintThreadSafe), Category = "Multi-Threading | Debug")
	static void LogThreadName();

	/**
	 * Gives actor posibility to run tick on any thread. Can be enabled or disabled.
	 * 
	 * @param TargetActor - Actor which you want to give ability run on any thread
	 * @param Enable - enable or disable run tick on any thread
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-Threading")
	static void EnableActorMultiThreadTick(AActor* TargetActor, bool Enable);

	/**
	 * Gives actor component posibility to run tick on any thread. Can be enabled or disabled.
	 * 
	 * @param TargetActorComponent - Actor Component which you want to give ability run on any thread
	 * @param Enable - enable or disable run tick on any thread
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-Threading")
	static void EnableActorComponentMultiThreadTick(UActorComponent* TargetActorComponent, bool Enable);

	/**
	 *	This node execute one function on background thread after which another one function will be executed on game thread.
	 * 
	 * @param BackGroundThreadLogic - Function which will be executed on background thread
	 * @param GameThreadLogic - Function which will be executed on game thread after background thread task done
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-Threading")
	static void RunTask_OnBackgroundThreadCallback(FFunctionThreadLogic BackGroundThreadLogic, FFunctionThreadLogic GameThreadLogic);

	/**
	 * This node execute one function on background thread.
	 * 
	 * @param BackGroundThreadLogic - Function which will be executed on background thread
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-Threading")
	static void RunTask_OnBackgroundThread(FFunctionThreadLogic BackGroundThreadLogic);

	/**
	 * This node execute one function on game thread.
	 * 
	 * @param GameThreadLogic - Function which will be executed on game thread
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-Threading")
	static void RunTask_OnGameThread(FFunctionThreadLogic GameThreadLogic);

	/**
	 * Run one function on multiple threads (function will have it loop index).
	 *
	 * WARNING!!!
	 * Don't use with events, use only with function. Usage with event will cause undefined behavior, crash, bugs.
	 *
	 * Loop code also will be executed on thread which call it (next Caller). If Caller is Game thread then Game thread
	 * will wait until all loops done (so can cause stutters). Better call it on background thread.
	 * 
	 * @param ParallelForLogic - Function which will be executed on many threads
	 * @param LoopAmount - amount of loops
	 * @param ParallelType - which parallel type to use
	 */
	UFUNCTION(BlueprintCallable, Category = "Multi-Threading")
	static void Run_ParallelFor(FParallelForLogic ParallelForLogic, int LoopAmount, EParallelFlags ParallelType = BackgroundPriority);
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThreadLogic);

UCLASS()
class BP_MULTITHREADING_API UBackgroundTaskCallbackNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnBackgroundThread;

	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnGameThread;
	
	virtual void Activate() override;
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Multi-Threading | Latent")
	static UBackgroundTaskCallbackNode* RunTask_OnBackgroundThreadCallback_Latent(UObject* WorldContextObject);

	
private:
	UPROPERTY()
	UObject* WorldContextObject;
};

UCLASS()
class BP_MULTITHREADING_API UScopedMutexLockNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FThreadLogic LockedMutex;

	FName MutexIdentifier;

	EMutexType MutexToUse;
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Multi-Threading | Latent")
	static UScopedMutexLockNode* RunLogicWithLockedMutex(UObject* WorldContextObject, FName NewMutexIdentifier, EMutexType NewMutexToUse);

	
private:
	UPROPERTY()
	UObject* WorldContextObject;
};

UCLASS()
class BP_MULTITHREADING_API UGameThreadTaskNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnGameThread;
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Multi-Threading | Latent")
	static UGameThreadTaskNode* RunTask_OnGameThread_Latent(UObject* WorldContextObject);

	
private:
	UPROPERTY()
	UObject* WorldContextObject;
};

UCLASS()
class BP_MULTITHREADING_API UBackgroundTaskNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FThreadLogic OnBackgroundThread;
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Multi-Threading | Latent")
	static UBackgroundTaskNode* RunTask_OnBackgroundThread_Latent(UObject* WorldContextObject);

	
private:
	UPROPERTY()
	UObject* WorldContextObject;
};
