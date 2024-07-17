// Copyright Andrii Loborchuk. All Rights Reserved. (2024)


#include "ActorComponentThread.h"

#include "ThreadedActorComponent.h"

FActorComponentThread::FActorComponentThread(UThreadedActorComponent* AttachedObject) {
	TickEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	ThreadObject = AttachedObject;
};

bool FActorComponentThread::Init() {
	/* Should the thread start? */
	return true;
}

uint32 FActorComponentThread::Run() {
	FPlatformProcess::Sleep(0.1);
	
	while (!bShutdown) {

		ThreadObject->OnAnotherThreadTick.Broadcast();
		TickEvent->Wait();
	}
	return 0;
}

void FActorComponentThread::Exit() {
	UE_LOG(LogTemp, Warning, TEXT("Delete Actor Thread"))
	FGenericPlatformProcess::ReturnSynchEventToPool(TickEvent);
	TickEvent = nullptr;
}

void FActorComponentThread::Stop() {
	bShutdown = true;
}
