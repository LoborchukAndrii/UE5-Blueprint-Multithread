// Fill out your copyright notice in the Description page of Project Settings.

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
