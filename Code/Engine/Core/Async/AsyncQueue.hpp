//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <queue>
#include <mutex>

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
class AsyncQueue
{
public:
	AsyncQueue();
	~AsyncQueue();


	void enqueue(TYPE const &element);
	bool dequeue(TYPE* out);

private:
	std::queue<TYPE> m_queue;
	std::mutex m_mutex;
};

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
void AsyncQueue<TYPE>::enqueue(TYPE const& element)
{
	std::lock_guard<std::mutex> mutexLock(m_mutex);
	m_queue.push(element);
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename TYPE>
bool AsyncQueue<TYPE>::dequeue(TYPE *out)
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
