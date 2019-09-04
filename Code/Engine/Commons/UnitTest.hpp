#pragma once
#include <limits.h>
#include "Engine/Commons/EngineCommon.hpp"

typedef bool(*UnitTestCallBack)();
#define MAX_TESTS 1024
extern uint gTestCount;

//------------------------------------------------------------------------------------------------------------------------------
class UnitTest
{
public:
	UnitTest(char const* name, char const* category, uint priority, UnitTestCallBack unitTestCallback);
	
private:
	UnitTestCallBack m_callBack;
	char const* m_name;
	char const* m_category;
	uint m_priority;

	UnitTest* m_nextTest = nullptr;

public:
	inline UnitTest*	GetNextTest() { return m_nextTest; }
	inline uint			GetPriority() { return m_priority; }
	const char*			GetTestName() { return m_name; }
	const char*			GetCategory() { return m_category; }
	bool				RunTest(); 
};

#define UNITTEST( name, category, priority ) 	\
	static bool MACRO_COMBINE(__UnitTest_, __LINE__)(); 	\
	static UnitTest MACRO_COMBINE(__UnitTestObj_, __LINE__)(name, category, priority, MACRO_COMBINE(__UnitTest_, __LINE__)); \
	static bool MACRO_COMBINE(__UnitTest_, __LINE__)()

#define CONFIRM(result) if(!(result)) {return false;}

void UnitTestRunAllCategories(uint priority = UINT_MAX);

void UnitTestRun(const char* category, uint priority = UINT_MAX);