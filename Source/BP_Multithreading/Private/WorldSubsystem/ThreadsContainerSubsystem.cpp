// Copyright Andrii Loborchuk. All Rights Reserved. (2025)


#include "WorldSubsystem/ThreadsContainerSubsystem.h"
#include "UObject/UnrealTypePrivate.h"

bool UThreadsContainerSubsystem::RemoveAtomic(FName Identifier)
{
	//Already doesn't exist?
	if (!AtomicMap.Contains(Identifier)) return false;

		
	delete AtomicMap[Identifier];
	AtomicMap.Remove(Identifier);
			
	return true;
}


void UThreadsContainerSubsystem::SetAtomic(FName Identifier, UProperty* Value)
{
	if (AtomicMap.Contains(Identifier))
	{
		AtomicMap[Identifier]->store(Value);
		return;
	}
	
	AtomicMap.Add(Identifier, new std::atomic(Value));
}

void UThreadsContainerSubsystem::AddQueue(FName Identifier, UProperty* Value)
{
	if (Value != nullptr)
	{
		if (QueueMap.Contains(Identifier))
		{
			QueueMap[Identifier]->Enqueue(Value);
			return;
		}

		TQueue<UProperty*>* NewQueue = new TQueue<UProperty*>();
		NewQueue->Enqueue(Value);

		QueueMap.Add(Identifier, NewQueue);
	}
}

bool UThreadsContainerSubsystem::GetQueue(FName Identifier, UProperty*& Value)
{
	if (!QueueMap.Contains(Identifier)) return false;
			
	QueueMap[Identifier]->Dequeue(Value);
	if (QueueMap[Identifier]->IsEmpty())
	{
		RemoveQueue(Identifier);
	}
	return true;
}

void UThreadsContainerSubsystem::RemoveQueue(FName Identifier)
{
	//Already doesn't exist?
	if (!QueueMap.Contains(Identifier)) return;
		
	delete QueueMap[Identifier];
	QueueMap.Remove(Identifier);

}

void UThreadsContainerSubsystem::LockCriticalSection(FName Identifier, EMutexType MutexType)
{
	if (MutexType == EMutexType::CriticalSection)
	{
		if (auto MutexPtr = GetCriticalSectionByName(Identifier))
		{
			MutexPtr->Lock();
		}
	}
	else
	{
		if (auto MutexPtr = GetSpinLockByName(Identifier))
		{
			MutexPtr->Lock();
		}
	}
}

bool UThreadsContainerSubsystem::TryLockCriticalSection(FName Identifier, EMutexType MutexType)
{
	if (MutexType == EMutexType::CriticalSection)
	{
		if (auto MutexPtr = GetCriticalSectionByName(Identifier))
		{
			return MutexPtr->TryLock();
		}
	}
	else
	{
		if (auto MutexPtr = GetSpinLockByName(Identifier))
		{
			return MutexPtr->TryLock();
		}
	}
	
	return false;
}

void UThreadsContainerSubsystem::UnlockCriticalSection(FName Identifier, EMutexType MutexType)
{
	if (MutexType == EMutexType::CriticalSection)
	{
		if (auto MutexPtr = GetCriticalSectionByName(Identifier))
		{
			MutexPtr->Unlock();
		}
	}
	else
	{
		if (auto MutexPtr = GetSpinLockByName(Identifier))
		{
			MutexPtr->Unlock();
		}
	}
}

void UThreadsContainerSubsystem::WaitTriggerEvent(FName Identifier)
{
	if (auto EventPtr = EventMap.Find(Identifier))
	{
		(*EventPtr)->Wait();
	}
	else
	{
		FEvent* Event = FGenericPlatformProcess::GetSynchEventFromPool(false);

		EventMap.Add(Identifier, Event);

		Event->Wait();
	}
}

void UThreadsContainerSubsystem::TriggerEvent(FName Identifier)
{
	if (auto EventPtr = EventMap.Find(Identifier))
	{
		(*EventPtr)->Trigger();
	}
}

FCriticalSection* UThreadsContainerSubsystem::GetCriticalSectionByName(FName Identifier)
{
	if (auto Mutex = CriticalSectionMap.Find(Identifier))
	{
		return Mutex->Get();
	}

	auto Mutex = MakeShared<FCriticalSection>();
	CriticalSectionMap.Add(Identifier, Mutex);
	auto NewMutexPtr = CriticalSectionMap.Find(Identifier);
	
	return NewMutexPtr->Get();
}

UE::FSpinLock* UThreadsContainerSubsystem::GetSpinLockByName(FName Identifier)
{
	if (auto Mutex = SpinLockMap.Find(Identifier))
	{
		return Mutex->Get();
	}

	auto Mutex = MakeShared<UE::FSpinLock>();
	SpinLockMap.Add(Identifier, Mutex);
	auto NewMutexPtr = SpinLockMap.Find(Identifier);
	
	return NewMutexPtr->Get();
}

bool UThreadsContainerSubsystem::GetAtomic(FName Identifier, UProperty*& Value)
{
	if (!AtomicMap.Contains(Identifier)) return false;
			
	Value = AtomicMap[Identifier]->load();
	return true;
}

bool UThreadsContainerSubsystem::GetAtomicAndRemove(FName Identifier, UProperty*& Value)
{
	if (!AtomicMap.Contains(Identifier)) return false;
			
	Value = AtomicMap[Identifier]->load();
	
	delete AtomicMap[Identifier];
	AtomicMap.Remove(Identifier);
	
	return true;
}

void UThreadsContainerSubsystem::BeginDestroy()
{
	UObject::BeginDestroy();

	TArray<FName> Keys;
	AtomicMap.GetKeys(Keys);
	for (auto& Key : Keys)
	{
		delete AtomicMap[Key];
	}
	
	QueueMap.GetKeys(Keys);
	for (auto& Key : Keys)
	{
		delete QueueMap[Key];
	}

	EventMap.GetKeys(Keys);
	for (auto& Key : Keys)
	{
		FGenericPlatformProcess::ReturnSynchEventToPool(EventMap[Key]);
	}
}