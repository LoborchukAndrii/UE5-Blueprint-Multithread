// Copyright Andrii Loborchuk. All Rights Reserved. (2025)

#pragma once

#include "MutexType.generated.h"

UENUM(BlueprintType)
enum EMutexType
{
	CriticalSection,
	SpinLock
};


