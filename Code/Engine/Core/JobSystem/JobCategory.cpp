#include "Engine/Core/JobSystem/JobCategory.hpp"

//------------------------------------------------------------------------------------------------------------------------------
void JobCategory::Enqueue(Job* job)
{
	m_pendingQueue.EnqueueLocked(job);
}

//------------------------------------------------------------------------------------------------------------------------------
Job* JobCategory::TryDequeue()
{
	Job* jobReturned = nullptr;
	m_pendingQueue.DequeueLocked(&jobReturned);

	return jobReturned;
}

//------------------------------------------------------------------------------------------------------------------------------
Job* JobCategory::Dequeue()
{
	Job* jobReturned = nullptr;
	bool result = false;

	while (!result)
	{
		result = m_pendingQueue.DequeueLocked(&jobReturned);
	}

	return jobReturned;
}

//------------------------------------------------------------------------------------------------------------------------------
void JobCategory::EnqueueFinished(Job* job)
{
	m_finishQueue.EnqueueLocked(job);
}

//------------------------------------------------------------------------------------------------------------------------------
Job* JobCategory::TryDequeueFinished()
{
	Job* jobReturned = nullptr;
	m_finishQueue.DequeueLocked(&jobReturned);

	return jobReturned;
}
