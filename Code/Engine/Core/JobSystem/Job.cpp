#include "Engine/Core/JobSystem/Job.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/JobSystem/JobSystem.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Job::Job()
{
	m_predecessorCount = 1;
}

//------------------------------------------------------------------------------------------------------------------------------
Job::~Job()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void Job::AddSuccessor(Job* job)
{
	job->AddPredecessor(this);
	//m_successors.push_back(job);
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::AddPredecessor(Job* job)
{
	m_predecessorCount++;

	{
		std::scoped_lock lock(m_mutex);
		job->m_successors.push_back(this);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::Dispatch()
{
	TryStart();
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::SetFinishCallback(finishCallback callBack)
{
	m_finishCallback = callBack;
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::SetJobCategory(eJobCategory type)
{
	m_category = type;
}

//------------------------------------------------------------------------------------------------------------------------------
eJobCategory Job::GetJobCategory()
{
	return (eJobCategory)m_category;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Job::TryStart()
{
	int newNumPredecessors = --m_predecessorCount;
	ASSERT_RECOVERABLE(newNumPredecessors >= 0, "The number of predecessors is lesser than 0!");

	if (0 == newNumPredecessors) 
	{
		EnqueueForCategoryInSystem();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::EnqueueForCategoryInSystem()
{
	JobSystem* system = JobSystem::GetInstance();

	system->AddJobForCategory(this, m_category);
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::EnqueueFinishedForCategoryInSystem()
{
	JobSystem* system = JobSystem::GetInstance();
	
	system->AddFinishedJobForCategory(this, m_category);
}

//------------------------------------------------------------------------------------------------------------------------------
void Job::FinishJob()
{
	//Try Start all your successors
	std::vector<Job*>::iterator itr = m_successors.begin();
	while (itr != m_successors.end())
	{
		(*itr)->TryStart();
		itr++;
	}
	
	if (m_finishCallback != nullptr) 
	{
		EnqueueFinishedForCategoryInSystem();
	}
	else 
	{
		delete this;
	}
}
