// Copyright Andrii Loborchuk. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThreadedActor.generated.h"

class FActorThread;

UCLASS()
class BP_MULTITHREADING_API AThreadedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AThreadedActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void On_Background_Tick();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly)
	FString ThreadName;

	FActorThread* ThreadLogic;

	FRunnableThread* Thread;
	
};

class FActorThread : public FRunnable {
public:
	FActorThread(AThreadedActor* AttachedActor);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	AThreadedActor* ThreadActor;
	FEvent* TickEvent;
	bool bShutdown = false;
	
};