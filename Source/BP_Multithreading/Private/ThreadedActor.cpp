// Copyright Andrii Loborchuk. All Rights Reserved.


#include "ThreadedActor.h"

AThreadedActor::AThreadedActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bRunOnAnyThread = true;

}

void AThreadedActor::BeginPlay()
{
	Super::BeginPlay();

	ThreadLogic = new FActorThread(this);
	if (ThreadName.Len() == 0)
	{
		ThreadName = GetActorNameOrLabel();
	}
	Thread = FRunnableThread::Create(ThreadLogic, *ThreadName);
}

void AThreadedActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	ThreadLogic->Stop();
	ThreadLogic->TickEvent->Trigger();
	Thread->Kill();
	delete ThreadLogic;
}

void AThreadedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ThreadLogic->TickEvent->Trigger();
}

FActorThread::FActorThread(AThreadedActor* AttachedActor) {
	TickEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	ThreadActor = AttachedActor;
};

bool FActorThread::Init() {
	/* Should the thread start? */
	return true;
}

uint32 FActorThread::Run() {
	FPlatformProcess::Sleep(0.1);
	
	while (!bShutdown) {

		ThreadActor->On_Background_Tick();
		TickEvent->Wait();
	}
	return 0;
}

void FActorThread::Exit() {
	UE_LOG(LogTemp, Warning, TEXT("Delete Actor Thread"))
	FGenericPlatformProcess::ReturnSynchEventToPool(TickEvent);
	TickEvent = nullptr;
}

void FActorThread::Stop() {
	bShutdown = true;
}
