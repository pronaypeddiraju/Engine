#include "Engine/Commons/UnitTest.hpp"

uint gTestCount = 0;
UnitTest* g_allUnitTests = nullptr;

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
				DebuggerPrintf("\n The test %s passed successfully \n", testIterator->GetTestName());
				++passed;
			}
			else
			{
				DebuggerPrintf("\n The test %s failed \n", testIterator->GetTestName());
			}
		}
		else
		{
			DebuggerPrintf("\n The test %s was of low priority \n", testIterator->GetTestName());
		}

		testIterator = testIterator->GetNextTest();
	}

	ASSERT_OR_DIE(total == passed, "\n The number of tests that passed are not the total number of tests \n");
	DebuggerPrintf("\n Tests passed : %d / %d \n", passed, total);
}

//------------------------------------------------------------------------------------------------------------------------------
void UnitTestRun(const char* category, uint priority /*= UINT_MAX*/)
{
	uint total = 0;
	uint passed = 0;

	UnitTest* testIterator = g_allUnitTests;
	while (nullptr != testIterator)
	{
		if (testIterator->GetPriority() < priority)
		{
			if (testIterator->GetCategory() == category)
			{
				++total;

				bool result = testIterator->RunTest();
				if (result)
				{
					DebuggerPrintf("\n The test %s passed successfully \n", testIterator->GetTestName());
					++passed;
				}
				else
				{
					DebuggerPrintf("\n The test %s failed \n", testIterator->GetTestName());
				}
			}
			else
			{
				DebuggerPrintf("\n The test %s was not under the %s category \n", testIterator->GetTestName(), testIterator->GetCategory());
			}
		}
		else
		{
			DebuggerPrintf("\n The test %s was of low priority \n", testIterator->GetTestName());
		}

		testIterator = testIterator->GetNextTest();
	}

	ASSERT_OR_DIE(total == passed, "\n The number of tests that passed are not the total number of tests \n");
	DebuggerPrintf("\n Tests passed : %d / %d \n", passed, total);
}

//------------------------------------------------------------------------------------------------------------------------------
UnitTest::UnitTest(char const* name, char const* category, uint priority, UnitTestCallBack unitTestCallback)
	: m_name(name)
	, m_category(category)
	, m_priority(priority)
	, m_callBack(unitTestCallback)
{
	ASSERT_OR_DIE(gTestCount < MAX_TESTS, "The number of tests is greater than the max tests allowed");

	m_nextTest = g_allUnitTests;
	g_allUnitTests = this;
}

//------------------------------------------------------------------------------------------------------------------------------
bool UnitTest::RunTest()
{
	return m_callBack();
}
