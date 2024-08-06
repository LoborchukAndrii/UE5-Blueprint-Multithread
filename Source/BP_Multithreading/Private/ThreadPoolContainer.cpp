// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadPoolContainer.h"
#include "Async/Async.h"
#include "Engine/Engine.h"

void UPoolQueueWork::DoThreadedWork()
{
	OnDoThreadedWork();
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnPostWork();
		MarkAsGarbage();
	});
}

void UPoolQueueWork::Abandon()
{
	MarkAsGarbage();
}

void UThreadPoolContainer::CreateThreadPool(const int ThreadsAmount, const EBPThreadPriority ThreadPriority, FString ThreadPoolName)
{
	ThreadPool = FQueuedThreadPool::Allocate();
	EThreadPriority Priority = TPri_Normal;
	switch (ThreadPriority)
	{
	case Normal:
		Priority = TPri_Normal;
		break;
	case AboveNormal:
		Priority = TPri_AboveNormal;
		break;
	case BelowNormal:
		Priority = TPri_BelowNormal;
		break;
	case Highest:
		Priority = TPri_Highest;
		break;
	case Lowest:
		Priority = TPri_Lowest;
		break;
	case SlightlyBelowNormal:
		Priority = TPri_SlightlyBelowNormal;
		break;
	case TimeCritical:
		Priority = TPri_Normal;
		break;
	case Num:
		Priority = TPri_TimeCritical;
		break;
	default:
		;
	}
	verify(ThreadPool->Create(ThreadsAmount, 32 * 1024, Priority, *ThreadPoolName));
}

void UThreadPoolContainer::AddWork(UPoolQueueWork* Work)
{
	if (ThreadPool && Work)
	{
		ThreadPool->AddQueuedWork(Work);
	}
}

bool UThreadPoolContainer::RemoveQueueWork(UPoolQueueWork* Work, const bool ForceClear)
{
	if (ThreadPool && Work)
	{
		bool Result = ThreadPool->RetractQueuedWork(Work);
		if (Result)
		{
			Work->MarkAsGarbage();
			if (ForceClear)
			{
				GEngine->ForceGarbageCollection();
			}
		}
		return Result;
	}
	return false;
}
