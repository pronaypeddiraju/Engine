#pragma once
#include "Engine/Core/JobSystem/Job.hpp"
#include "Engine/Core/Async/AsyncQueue.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class JobCategory
{
public:
	void				Enqueue(Job* job);
	Job*				TryDequeue();			// returns nullptr if no job is ready(I.e fails if the queue is empty)
	Job*				Dequeue();				// returns nullptr only if system is shutdown, otherwise block until a job is ready for use (I.e waits till a job was added to queue)

	void				EnqueueFinished(Job* job);
	Job*				TryDequeueFinished();

private:
	AsyncQueue<Job*>	m_pendingQueue;
	AsyncQueue<Job*>	m_finishQueue;
};