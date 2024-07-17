// Copyright Andrii Loborchuk. All Rights Reserved. (2024)

#pragma once

#include "CoreMinimal.h"

class UThreadedActorComponent;

class FActorComponentThread : public FRunnable {
public:
	FActorComponentThread(UThreadedActorComponent* AttachedObject);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	UThreadedActorComponent* ThreadObject;
	FEvent* TickEvent;
	bool bShutdown = false;
	
};
