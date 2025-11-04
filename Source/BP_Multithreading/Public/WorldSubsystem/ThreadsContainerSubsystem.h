// Copyright Andrii Loborchuk. All Rights Reserved. (2025)

#pragma once

#include "CoreMinimal.h"
#include "Data/MutexType.h"
#include "Misc/SpinLock.h"
#include "Subsystems/WorldSubsystem.h"
#include "ThreadsContainerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BP_MULTITHREADING_API UThreadsContainerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
#pragma region Atomic
	UFUNCTION(BlueprintCallable, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	void SetAtomic(FName Identifier, UProperty* Value);

	UFUNCTION(BlueprintPure, meta=(CustomStructureParam = "Value"), Category = "Atomics|Get")
	bool GetAtomic(FName Identifier, UProperty*& Value);

	UFUNCTION(BlueprintPure, meta=(CustomStructureParam = "Value"), Category = "Atomics|Get")
	bool GetAtomicAndRemove(FName Identifier, UProperty*& Value);
	
	UFUNCTION(BlueprintCallable, Category="Atomics|Remove")
	bool RemoveAtomic(FName Identifier);

	UFUNCTION(BlueprintCallable, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	void AddQueue(FName Identifier, UProperty* Value);

	UFUNCTION(BlueprintPure, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	bool GetQueue(FName Identifier, UProperty*& Value);

	UFUNCTION(BlueprintCallable, meta=(CustomStructureParam = "Value"), Category = "Atomics|Set")
	void RemoveQueue(FName Identifier);
#pragma endregion

#pragma region Mutex
	/**
	 * Thread will wait until mutex unlocked and locked it after get it posibility. Needed manually unlock. 
	 * 
	 * @param Identifier - Mutex name.
	 * @param MutexType - Mutex type (have different behavior of wait).
	 */
	UFUNCTION(BlueprintCallable, Category = "Mutex")
	void LockCriticalSection(FName Identifier, EMutexType MutexType);

	/**
	 * Function will try to lock mutex, if its successful (needed manually unlock) then it will lock mutex otherwise return false.
	 * 
	 * @param Identifier - Mutex name.
	 * @param MutexType - Mutex type (have different behavior of wait).
	 * @return Returning is mutes gets locked or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mutex")
	bool TryLockCriticalSection(FName Identifier, EMutexType MutexType);

	/**
	 * Unlock mutex.
	 * 
	 * @param Identifier - Mutex name.
	 * @param MutexType - Mutex type (have different behavior of wait).
	 */
	UFUNCTION(BlueprintCallable, Category = "Mutex")
	void UnlockCriticalSection(FName Identifier, EMutexType MutexType);

	/**
	 * Thread will wait until event triggered.
	 * 
	 * @param Identifier - Event name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mutex")
	void WaitTriggerEvent(FName Identifier);

	/**
	 * Trigger event so another thread can go on.
	 * 
	 * @param Identifier - Event name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mutex")
	void TriggerEvent(FName Identifier);

	FCriticalSection* GetCriticalSectionByName(FName Identifier);
	UE::FSpinLock* GetSpinLockByName(FName Identifier);
#pragma endregion

	
	virtual void BeginDestroy() override;

private:
	TMap<FName, TSharedPtr<FCriticalSection>> CriticalSectionMap;
	TMap<FName, TSharedPtr<UE::FSpinLock>> SpinLockMap;
	TMap<FName, FEvent*> EventMap;
	
	TMap<FName, std::atomic<UProperty*>*> AtomicMap;
	TMap<FName, TQueue<UProperty*>*> QueueMap;
};

