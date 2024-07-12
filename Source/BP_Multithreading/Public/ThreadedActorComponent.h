// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <variant>

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

#pragma region Atomic
	UFUNCTION(BlueprintCallable, Category="Atomics|Set")
	void SetAtomicInteger(FName Identifier, int Value) {SetAtomic(Identifier, Value);}
	UFUNCTION(BlueprintPure, Category="Atomics|Get")
	bool GetAtomicInteger(FName Identifier, int& Value) {return GetAtomic(Identifier, Value);}

	UFUNCTION(BlueprintCallable, Category="Atomics|Set")
	void SetAtomicFloat(FName Identifier, float Value) {SetAtomic(Identifier, Value);}
	UFUNCTION(BlueprintPure, Category="Atomics|Get")
	bool GetAtomicFloat(FName Identifier, float& Value) {return GetAtomic(Identifier, Value);}

	UFUNCTION(BlueprintCallable, Category="Atomics|Set")
	void SetAtomicBool(FName Identifier, bool Value) {SetAtomic(Identifier, Value);}
	UFUNCTION(BlueprintPure, Category="Atomics|Get")
	bool GetAtomicBool(FName Identifier, bool& Value) {return GetAtomic(Identifier, Value);}

	UFUNCTION(BlueprintCallable, Category="Atomics|Remove")
	bool RemoveAtomic(FName Identifier);
#pragma endregion

protected:
	virtual void BeginPlay() override;

private:	
	FActorComponentThread* ThreadLogic;

    FRunnableThread* Thread;

#pragma region Atomic
	#define VariableTypes int, float, bool
	TMap<FName, std::atomic<std::variant<VariableTypes>>*> AtomicMap;
	TMap<FName, FString> AtomicMapTypes;

	template <typename Type>
	void SetAtomic(FName Identifier, Type Value);

	template <typename Type>
	bool GetAtomic(FName Identifier, Type& Value);
#pragma endregion

};
