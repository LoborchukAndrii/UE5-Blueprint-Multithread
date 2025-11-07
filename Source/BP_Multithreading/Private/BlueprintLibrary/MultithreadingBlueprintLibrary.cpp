// Copyright Andrii Loborchuk. All Rights Reserved. (2024)


#include "BlueprintLibrary/MultithreadingBlueprintLibrary.h"
#include "HAL/ThreadManager.h"
#include "Async/Async.h"
#include "GameFramework/Actor.h"
#include "Async/ParallelFor.h"
#include "Engine/World.h"
#include "WorldSubsystem/ThreadsContainerSubsystem.h"

DEFINE_LOG_CATEGORY(LogThread);

void UMultithreadingBlueprintLibrary::SleepThread(float SleepTime)
{
	FPlatformProcess::Sleep(SleepTime);
}

void UMultithreadingBlueprintLibrary::LogThreadName()
{
	uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
	FString ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
	UE_LOG(LogThread, Warning, TEXT("%s"), *ThreadName)
}

void UMultithreadingBlueprintLibrary::EnableActorMultiThreadTick(AActor* TargetActor, bool Enable)
{
	TargetActor->PrimaryActorTick.bRunOnAnyThread = Enable;
}

void UMultithreadingBlueprintLibrary::EnableActorComponentMultiThreadTick(UActorComponent* TargetActorComponent,
	bool Enable)
{
	TargetActorComponent->PrimaryComponentTick.bRunOnAnyThread = Enable;
}

void UMultithreadingBlueprintLibrary::RunTask_OnBackgroundThreadCallback(FFunctionThreadLogic BackGroundThreadLogic, FFunctionThreadLogic GameThreadLogic)
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

void UMultithreadingBlueprintLibrary::RunTask_OnBackgroundThread(FFunctionThreadLogic BackGroundThreadLogic)
{
	AsyncTask(ENamedThreads::AnyThread, [BackGroundThreadLogic]()
	{
		BackGroundThreadLogic.Execute();
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

void UBackgroundTaskCallbackNode::Activate()
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

UBackgroundTaskCallbackNode* UBackgroundTaskCallbackNode::RunTask_OnBackgroundThreadCallback_Latent(UObject* WorldContextObject)
{
	UBackgroundTaskCallbackNode* BlueprintNode = NewObject<UBackgroundTaskCallbackNode>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UScopedMutexLockNode::Activate()
{
	Super::Activate();

	if (WorldContextObject->GetWorld())
	{
		if (UThreadsContainerSubsystem* TCS = WorldContextObject->GetWorld()->GetSubsystem<UThreadsContainerSubsystem>())
		{
			if (MutexToUse == EMutexType::CriticalSection)
			{
				auto Mutex = TCS->GetCriticalSectionByName(MutexIdentifier);
				if (Mutex)
				{
					FScopeLock Lock(Mutex);
			
					LockedMutex.Broadcast();
				}
			}
			else
			{
				auto Mutex = TCS->GetSpinLockByName(MutexIdentifier);
				if (Mutex)
				{
					Mutex->Lock();
			
					LockedMutex.Broadcast();
					
					Mutex->Unlock();
				}
			}
		}
	}
}

UScopedMutexLockNode* UScopedMutexLockNode::RunLogicWithLockedMutex(UObject* WorldContextObject, FName NewMutexIdentifier, EMutexType NewMutexToUse)
{
	UScopedMutexLockNode* BlueprintNode = NewObject<UScopedMutexLockNode>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->MutexIdentifier = NewMutexIdentifier;
	BlueprintNode->MutexToUse = NewMutexToUse;
	return BlueprintNode;
}

void UGameThreadTaskNode::Activate()
{
	Super::Activate();

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			OnGameThread.Broadcast();
			SetReadyToDestroy();
		});
}

UGameThreadTaskNode* UGameThreadTaskNode::RunTask_OnGameThread_Latent(UObject* WorldContextObject)
{
	UGameThreadTaskNode* BlueprintNode = NewObject<UGameThreadTaskNode>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UBackgroundTaskNode::Activate()
{
	Super::Activate();

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		OnBackgroundThread.Broadcast();
		SetReadyToDestroy();
	});
}

UBackgroundTaskNode* UBackgroundTaskNode::RunTask_OnBackgroundThread_Latent(UObject* WorldContextObject)
{
	UBackgroundTaskNode* BlueprintNode = NewObject<UBackgroundTaskNode>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}
