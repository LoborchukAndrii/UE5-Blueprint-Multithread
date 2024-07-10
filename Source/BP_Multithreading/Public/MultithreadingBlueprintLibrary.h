// Copyright Andrii Loborchuk. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultithreadingBlueprintLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogThread, Log, All);

/**
 * 
 */
UCLASS()
class BP_MULTITHREADING_API UMultithreadingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void SleepThread(float SleepTime);

	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void GetThread();

	UFUNCTION(BlueprintCallable, Category = "Threaded")
	static void EnableActorMultiThreadTick(AActor* TargetActor);

	
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
