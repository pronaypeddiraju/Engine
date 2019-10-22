#include "Engine/Core/JobSystem/JobSystem.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/JobSystem/Job.hpp"
#include "Engine/Core/Time.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

JobSystem* gJobSystem = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
JobSystem* JobSystem::CreateInstance()
{
	if (gJobSystem == nullptr)
	{
		gJobSystem = new JobSystem();
	}

	gJobSystem->Startup(-2, gJobSystem->m_numCategories);

	return gJobSystem;
}

//------------------------------------------------------------------------------------------------------------------------------
JobSystem* JobSystem::GetInstance()
{
	if (gJobSystem == nullptr)
	{
		JobSystem::CreateInstance();
	}
	
	return gJobSystem;
}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::DestroyInstance()
{
	if (gJobSystem != nullptr)
	{
		gJobSystem->Shutdown();
		delete gJobSystem;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::Startup(int numGenericThreads /*= -1*/, int numCategories /*= JOB_CATEGORY_CORE_COUNT*/)
{
	m_genericJobsSemaphore.Create(0, 1);
	m_isRunning = true;

	//Create required number of JobCategories
	m_categories = new JobCategory[numCategories];

	// figure out thread count
	int coreCount = std::thread::hardware_concurrency();
	int numThreadsToMake = coreCount;
	if (numGenericThreads < 0)
	{
		numThreadsToMake += numGenericThreads;

		//Case where numThreadsToMake is still negative
		if (numThreadsToMake < 0)
			numThreadsToMake = 1;
	}
	else
	{
		numThreadsToMake = numGenericThreads;
	}

	for (int threadIndex = 0; threadIndex < numThreadsToMake; threadIndex++)
	{
		//Make these threads run the generic work task
		m_genericThreads.emplace_back(&GenericThreadWork);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::Shutdown()
{
	m_isRunning = false;

	for (int threadIndex = 0; threadIndex < m_genericThreads.size(); threadIndex++)
	{
		m_genericThreads[threadIndex].join();
	}

}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::Run(Job* job)
{
	job->TryStart();
}

//------------------------------------------------------------------------------------------------------------------------------
bool JobSystem::ProcessCategoryForTimeInMS(int category, uint ms)
{
	double startTime = GetCurrentTimeSeconds() * 1000;
	double endTime = 0;
	bool elapsed = false;
	bool result = false;

	while (!elapsed)
	{
		result = ProcessCategory(category);
		if (!result)
		{
			elapsed = true;
		}

		endTime = GetCurrentTimeSeconds() * 1000;
		if (endTime > startTime + ms)
		{
			elapsed = true;
		}
	}

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
bool JobSystem::ProcessCategory(int category)
{
	Job* job = nullptr;

	job = m_categories[category].TryDequeue();

	if (job != nullptr)
	{
		job->Execute();
		job->FinishJob();

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::ProcessFinishJobsForCategory(int category)
{
	Job* job = m_categories[category].TryDequeueFinished();

	while (job)
	{
		job->m_finishCallback(job);

		delete job;
		job = nullptr;

		job = m_categories[category].TryDequeueFinished();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::AddJobForCategory(Job* job, int category)
{
	m_categories[category].Enqueue(job);
	SignalWork();
}

//------------------------------------------------------------------------------------------------------------------------------
void JobSystem::AddFinishedJobForCategory(Job* job, int category)
{
	m_categories[category].EnqueueFinished(job);
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC void JobSystem::GenericThreadWork()
{
	//This is the work the generic thread needs to do
	JobSystem* system = JobSystem::GetInstance();
	while (system->m_isRunning)
	{
		system->WaitForWork();
		while (system->ProcessCategoryForTimeInMS(JOB_GENERIC, 5));

		system->ProcessFinishJobsForCategory(JOB_GENERIC);
		Sleep(0);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
