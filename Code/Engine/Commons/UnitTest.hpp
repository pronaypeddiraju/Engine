#pragma once
#include <limits.h>
#include "Engine/Commons/EngineCommon.hpp"

#define UNITTEST(name, category, priority)

#define CONFIRM(result) if(!(result)) {return false;}

void UnitTestRunAllCategories(uint priority = UINT_MAX);

void UnitTestRun(const char* category, uint priority = UINT_MAX);