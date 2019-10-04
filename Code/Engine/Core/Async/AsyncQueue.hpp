//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <queue>
#include <mutex>

//------------------------------------------------------------------------------------------------------------------------------
// NOTE: This data structure uses blocked enque and dequeue. This mean that the lock is always going to block the thread
// It will not perform a try_lock which means that the enque and dequeue will wait till the thread is available to perform
// actions on the queue
//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
class AsyncQueue
{
public:
	AsyncQueue();
	~AsyncQueue();


	void EnqueueLocked(TYPE const &element);
	bool DequeueLocked(TYPE* out);

	int GetLength() const;

private:
	std::queue<TYPE> m_queue;
	std::mutex m_mutex;
};

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
int AsyncQueue<TYPE>::GetLength() const
{
	std::lock_guard<std::mutex> mutexLock(m_mutex);
	return m_queue.size();
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
AsyncQueue<TYPE>::AsyncQueue()
{

}

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
AsyncQueue<TYPE>::~AsyncQueue()
{

}

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
void AsyncQueue<TYPE>::EnqueueLocked(TYPE const& element)
{
	std::lock_guard<std::mutex> mutexLock(m_mutex);
	m_queue.push(element);
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
bool AsyncQueue<TYPE>::DequeueLocked(TYPE *out)
{
	std::lock_guard<std::mutex> mutexLock(m_mutex);
	if (m_queue.empty()) 
	{
		return false;
	}
	else 
	{
		*out = m_queue.front();
		m_queue.pop();
		return true;
	}
}
