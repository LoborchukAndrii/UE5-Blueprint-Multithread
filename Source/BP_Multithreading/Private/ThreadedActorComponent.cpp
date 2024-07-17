// Copyright Andrii Loborchuk. All Rights Reserved. (2024)


#include "ThreadedActorComponent.h"
#include "ActorComponentThread.h"

UThreadedActorComponent::UThreadedActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = true;

}


bool UThreadedActorComponent::RemoveAtomic(FName Identifier)
{
	//Already doesn't exist?
	if (!AtomicMap.Contains(Identifier)) return false;

		
	delete AtomicMap[Identifier];
	AtomicMap.Remove(Identifier);
	AtomicMapTypes.Remove(Identifier);
			
	return true;
}

void UThreadedActorComponent::BeginPlay()
{
	Super::BeginPlay();

	ThreadLogic = new FActorComponentThread(this);
	if (IsValid(GetOwner()))
	{
		Thread = FRunnableThread::Create(ThreadLogic, *GetOwner()->GetActorNameOrLabel());
	}
}

void UThreadedActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ThreadLogic->Stop();
	ThreadLogic->TickEvent->Trigger();
	Thread->Kill();
	delete ThreadLogic;

	// We need to delete this data to clear the memory because the component has completed its lifecycle.
	// This deletion is necessary because we created it with new.
	TArray<FName> Keys;
	AtomicMap.GetKeys(Keys);
	for (auto Key : Keys)
	{
		delete AtomicMap[Key];
	}
}

void UThreadedActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ThreadLogic->TickEvent->Trigger();
}

template <typename Type>
void UThreadedActorComponent::SetAtomic(FName Identifier, Type Value)
{
	FString CurrentTypeName = typeid(Type).name();
		
	//Identifier exists?
	if (AtomicMap.Contains(Identifier))
	{
		//Update identifier
		AtomicMap[Identifier]->store(Value);
		AtomicMapTypes[Identifier] = CurrentTypeName;

		return;
	}


	//Can only reach here if owner or identifier didn't exist
	AtomicMap.Add(Identifier, new std::atomic(std::variant<VariableTypes>(Value)));
	AtomicMapTypes.Add(Identifier, CurrentTypeName);
}

template <typename Type>
bool UThreadedActorComponent::GetAtomic(FName Identifier, Type& Value)
{
	//Doesn't exist? Exit
	if (!AtomicMap.Contains(Identifier)) return false;

		
	//Different type?
	FString PassedType = typeid(Type).name();
	if (AtomicMapTypes[Identifier] != PassedType)
	{
		GEngine->AddOnScreenDebugMessage(0, 20, FColor::Red,FString::Printf(TEXT
											 ("Tried to access atomic \"%ls\" with type \"%ls\" when it's defined as \"%ls\""),
												 *Identifier.ToString(),  *PassedType, *AtomicMapTypes[Identifier]));
		return false;
	}
			
	Value = std::get<Type>(AtomicMap[Identifier]->load());
	return true;
}
