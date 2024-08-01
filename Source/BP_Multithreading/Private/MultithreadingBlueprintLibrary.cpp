// Copyright Andrii Loborchuk. All Rights Reserved. (2024)


#include "MultithreadingBlueprintLibrary.h"
#include "HAL/ThreadManager.h"
#include "Async/Async.h"
#include "GameFramework/Actor.h"
#include "Async/ParallelFor.h"

DEFINE_LOG_CATEGORY(LogThread);

void UMultithreadingBlueprintLibrary::SleepThread(float SleepTime)
{
	FPlatformProcess::Sleep(SleepTime);
}

void UMultithreadingBlueprintLibrary::GetThread()
{
	uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
	FString ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
	UE_LOG(LogThread, Warning, TEXT("%s"), *ThreadName)
}

void UMultithreadingBlueprintLibrary::EnableActorMultiThreadTick(AActor* TargetActor)
{
	TargetActor->PrimaryActorTick.bRunOnAnyThread = true;
}

void UMultithreadingBlueprintLibrary::RunTask_OnBackgroundThread(FFunctionThreadLogic BackGroundThreadLogic, FFunctionThreadLogic GameThreadLogic)
{
	AsyncTask(ENamedThreads::AnyThread, [BackGroundThreadLogic, GameThreadLogic]()
	{
		BackGroundThreadLogic.Execute();
		
		AsyncTask(ENamedThreads::GameThread, [GameThreadLogic]()
		{
			GameThreadLogic.Execute();
		});
	});
}

void UMultithreadingBlueprintLibrary::RunTask_OnGameThread(FFunctionThreadLogic GameThreadLogic)
{
	AsyncTask(ENamedThreads::GameThread, [GameThreadLogic]()
		{
			GameThreadLogic.Execute();
		});
}

void UMultithreadingBlueprintLibrary::Run_ParallelFor(FParallelForLogic ParallelForLogic, int LoopAmount, EParallelFlags ParallelType)
{
	EParallelForFlags ParallelFlag = EParallelForFlags::None;
	switch (ParallelType)
	{
		case None:
			ParallelFlag = EParallelForFlags::None;
			break;
		case ForceSingleThread:
			ParallelFlag = EParallelForFlags::ForceSingleThread;
			break;
		case Unbalanced:
			ParallelFlag = EParallelForFlags::Unbalanced;
			break;
		case PumpRenderingThread:
			ParallelFlag = EParallelForFlags::PumpRenderingThread;
			break;
		case BackgroundPriority:
			ParallelFlag = EParallelForFlags::BackgroundPriority;
			break;
		default:
			;
	}
	ParallelFor(LoopAmount, [&ParallelForLogic](int Index)
	{
		FParallelForLogic LocalDelegate = ParallelForLogic;
		LocalDelegate.Execute(Index);
	}, ParallelFlag);
}

void UAsyncThread::Activate()
{
	Super::Activate();

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		OnBackgroundThread.Broadcast();
		
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			OnGameThread.Broadcast();
			SetReadyToDestroy();
		});
	});
}

UAsyncThread* UAsyncThread::Threaded_Logic(const UObject* WorldContextObject)
{
	UAsyncThread* BlueprintNode = NewObject<UAsyncThread>();
	BlueprintNode->WorldContextObject = const_cast<UObject*>(WorldContextObject);
	return BlueprintNode;
}

void UAsyncGameThread::Activate()
{
	Super::Activate();

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			OnGameThread.Broadcast();
			SetReadyToDestroy();
		});
}

UAsyncGameThread* UAsyncGameThread::GameThread_Logic(const UObject* WorldContextObject)
{
	UAsyncGameThread* BlueprintNode = NewObject<UAsyncGameThread>();
	BlueprintNode->WorldContextObject = const_cast<UObject*>(WorldContextObject);
	return BlueprintNode;
}
