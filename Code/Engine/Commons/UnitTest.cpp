#include "Engine/Commons/EngineCommon.hpp"

typedef bool(*UnitTestCallBack)();

#define MAX_TESTS 1024
uint gTestCount = 0;

//------------------------------------------------------------------------------------------------------------------------------
class UnitTest
{
	UnitTest(char const* name, char const* category, uint priority, UnitTestCallBack unitTestCallback)
		: m_name(name)
		, m_category(category)
		, m_priority(priority)
		, m_callBack(unitTestCallback)
	{
		ASSERT_OR_DIE(gTestCount < MAX_TESTS, "The number of tests is greater than the max tests allowed");

		m_nextTest = g_allUnitTests;
		g_allUnitTests = this;
	}

	UnitTestCallBack m_callBack;
	char const* m_name;
	char const* m_category;
	uint m_priority;

	UnitTest* m_nextTest = nullptr;

public:
	inline UnitTest*	GetNextTest() { return m_nextTest; }
	inline uint			GetPriority() { return m_priority; }
	const char*			GetTestName() { return m_name; }
	inline bool			RunTest() { return m_callBack(); }
};

//------------------------------------------------------------------------------------------------------------------------------
void UnitTestRunAllCategories(uint priority /*= UINT_MAX*/)
{
	uint total = 0;
	uint passed = 0;

	UnitTest* testIterator = g_allUnitTests;
	while (nullptr != testIterator) 
	{
		if (testIterator->GetPriority() < priority)
		{
			++total;

			bool result = testIterator->RunTest();
			if (result)
			{
				DebuggerPrintf("The test %s passed successfully", testIterator->GetTestName());
				++passed;
			}
			else
			{
				DebuggerPrintf("The test %s failed", testIterator->GetTestName());
			}
		}
		else
		{
			DebuggerPrintf("The test %s was of low priority", testIterator->GetTestName());
		}

		testIterator = testIterator->GetNextTest();
	}

	ASSERT_OR_DIE(total == passed, "The number of tests that passed are not the total number of tests");
}

//------------------------------------------------------------------------------------------------------------------------------
#define UNITTEST( name, category, priority ) 	\
	static bool MACRO_COMBINE(__UnitTest_, __LINE__)(); 	\
	static UnitTest MACRO_COMBINE(__UnitTestObj_, __LINE__)(name, category, priority, MACRO_COMBINE(__UnitTest_, __LINE__)); \
	static bool MACRO_COMBINE(__UnitTest_, __LINE__)() 	\
	
