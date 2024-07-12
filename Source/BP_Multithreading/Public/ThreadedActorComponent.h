// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThreadedActorComponent.generated.h"


class FActorComponentThread;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnotherThreadTickDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BP_MULTITHREADING_API UThreadedActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UThreadedActorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintAssignable, Category = "Threaded")
	FOnAnotherThreadTickDelegate OnAnotherThreadTick;

protected:
	virtual void BeginPlay() override;

private:	
	FActorComponentThread* ThreadLogic;

    FRunnableThread* Thread;
};
