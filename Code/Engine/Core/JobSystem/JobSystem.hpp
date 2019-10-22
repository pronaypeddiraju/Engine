#pragma once
#include "Engine/Core/JobSystem/JobTypes.hpp"
#include "Engine/Core/Async/Semaphores.hpp"
#include "Engine/Core/JobSystem/JobCategory.hpp"
#include <vector>
#include <thread>

typedef unsigned int uint;
class Job;

//------------------------------------------------------------------------------------------------------------------------------
class JobSystem
{
public:

	static JobSystem*	CreateInstance();
	static JobSystem*	GetInstance();
	static void			DestroyInstance();

	// negative here means as many as you can MINUS the value (8 cores, -1 is 7... -2 would be 6.  -20 would be 1)
	// MINIMUM 1 unless explicitly saying 0; 
	void				Startup(int numGenericThreads = -1, int numCategories = JOB_CATEGORY_CORE_COUNT);
	void				Shutdown();

	void				Run(Job* job);

	bool				ProcessCategoryForTimeInMS(int category, uint ms); // process until no more jobs, or until 'ms' has passed 
	bool				ProcessCategory(int category); // process until no more jobs, return number of jobs executed
	
	void				ProcessFinishJobsForCategory(int category);

	void				AddJobForCategory(Job* job, int category);
	void				AddFinishedJobForCategory(Job* job, int category);

private:
	static void			GenericThreadWork();

	Semaphore			m_genericJobsSemaphore;

	void				WaitForWork()	{ m_genericJobsSemaphore.Acquire(); }
	void				SignalWork()	{ m_genericJobsSemaphore.Release(1); }

	JobCategory*				m_categories;
	int							m_numCategories = JOB_CATEGORY_CORE_COUNT;

	std::vector<std::thread>	m_genericThreads;

	bool m_isRunning;
};