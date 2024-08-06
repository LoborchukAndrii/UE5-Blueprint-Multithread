// Fill out your copyright notice in the Description page of Project Settings.


#include "AtomicContainer.h"

bool UAtomicContainer::RemoveAtomic(FName Identifier)
{
	//Already doesn't exist?
	if (!AtomicMap.Contains(Identifier)) return false;

		
	delete AtomicMap[Identifier];
	AtomicMap.Remove(Identifier);
			
	return true;
}


void UAtomicContainer::SetAtomic(FName Identifier, UProperty* Value)
{
	if (AtomicMap.Contains(Identifier))
	{
		AtomicMap[Identifier]->store(Value);
		return;
	}
	
	AtomicMap.Add(Identifier, new std::atomic(Value));
}

void UAtomicContainer::AddQueue(FName Identifier, UProperty* Value)
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

bool UAtomicContainer::GetQueue(FName Identifier, UProperty*& Value)
{
	if (!QueueMap.Contains(Identifier)) return false;
			
	QueueMap[Identifier]->Dequeue(Value);
	if (QueueMap[Identifier]->IsEmpty())
	{
		RemoveQueue(Identifier);
	}
	return true;
}

void UAtomicContainer::RemoveQueue(FName Identifier)
{
	//Already doesn't exist?
	if (!QueueMap.Contains(Identifier)) return;
		
	delete QueueMap[Identifier];
	QueueMap.Remove(Identifier);

}

bool UAtomicContainer::GetAtomic(FName Identifier, UProperty*& Value)
{
	if (!AtomicMap.Contains(Identifier)) return false;
			
	Value = AtomicMap[Identifier]->load();
	return true;
}

bool UAtomicContainer::GetAtomicAndRemove(FName Identifier, UProperty*& Value)
{
	if (!AtomicMap.Contains(Identifier)) return false;
			
	Value = AtomicMap[Identifier]->load();
	
	delete AtomicMap[Identifier];
	AtomicMap.Remove(Identifier);
	
	return true;
}

void UAtomicContainer::BeginDestroy()
{
	UObject::BeginDestroy();

	TArray<FName> Keys;
	AtomicMap.GetKeys(Keys);
	for (auto Key : Keys)
	{
		delete AtomicMap[Key];
	}
	
	QueueMap.GetKeys(Keys);
	for (auto Key : Keys)
	{
		delete QueueMap[Key];
	}
}