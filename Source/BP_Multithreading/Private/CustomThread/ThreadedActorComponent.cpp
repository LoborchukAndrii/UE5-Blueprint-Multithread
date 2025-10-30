// Copyright Andrii Loborchuk. All Rights Reserved. (2024)


#include "ThreadedActorComponent.h"
#include "ActorComponentThread.h"
#include "HAL/RunnableThread.h"
#include "GameFramework/Actor.h"
#include "HAL/Runnable.h"
#include "typeinfo"

UThreadedActorComponent::UThreadedActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = true;

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
}

void UThreadedActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ThreadLogic->TickEvent->Trigger();
}