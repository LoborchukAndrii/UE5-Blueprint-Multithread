// Copyright Andrii Loborchuk. All Rights Reserved. (2025)

#pragma once

#include "MutexType.generated.h"

UENUM(BlueprintType)
enum EMutexType
{
	CriticalSection UMETA(ToolTip="Puts thread to sleep until mutex unlocked."),
	SpinLock UMETA(ToolTip="Thread spin in loop until mutex unlocked.")
};


