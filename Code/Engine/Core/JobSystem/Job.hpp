#pragma once
#include "Engine/Core/JobSystem/JobTypes.hpp"
#include <functional>
#include <atomic>
#include <mutex>

//------------------------------------------------------------------------------------------------------------------------------
class Job
{
	friend class JobSystem;

public:
	Job();
	~Job();

	using finishCallback = std::function<void(Job*)>;

	void				AddSuccessor(Job* job);
	void				AddPredecessor(Job* job);
	void				Dispatch();

	//NOTE: 
	//	To add a methods to callback
	//	job->set_callback( [=]() { this->apply_path( job->path ); } ); 
	//	Otherwise we can simply bind static functions or stand alone functions with no extra lambda magic
	void				SetFinishCallback(finishCallback callBack);

	void				SetJobCategory(eJobCategory type);
	eJobCategory		GetJobCategory();
	virtual void		Execute() = 0;

private:
	bool				TryStart();
	void				EnqueueForCategoryInSystem();
	void				EnqueueFinishedForCategoryInSystem();
	void				FinishJob();

	int 				m_category = JOB_GENERIC;

	std::vector<Job*> 	m_successors;
	std::atomic<int> 	m_predecessorCount;

	// Options - support at least one callback version
	finishCallback		m_finishCallback;
	std::string 		m_event;

	std::mutex			m_mutex;
};