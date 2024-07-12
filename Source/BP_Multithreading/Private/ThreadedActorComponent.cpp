// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadedActorComponent.h"
#include "ActorComponentThread.h"

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

