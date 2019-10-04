#pragma once
#include <array>
#include <mutex>

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
class UniformAsyncRingBuffer
{
public:
	explicit UniformAsyncRingBuffer(size_t bufferSize);

	void			Insert(T item);
	T				ReadBuffer();
	void			ResetBuffer();

	bool			IsEmpty() const;
	bool			IsFull() const;
	size_t			GetCapacity() const;
	size_t			GetSize() const;

private:
	std::mutex						m_lock;
	//std::array<T, 0>				m_buffer;
	std::vector<T>					m_buffer;
	size_t							m_head = 0;
	size_t							m_tail = 0;
	const size_t					m_maxSize = 0;
	bool							m_isFull = false;
};

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
T UniformAsyncRingBuffer<T>::ReadBuffer()
{
	std::lock_guard<std::mutex> lock(m_lock);

	//If buffer is empty, just return an empty object of type T
	if (IsEmpty())
	{
		return T();
	}

	//Read data and advance the tail (we now have a free space)
	auto value = m_buffer[m_tail];
	m_isFull = false;
	m_tail = (m_tail + 1) % m_maxSize;

	return value;
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
void UniformAsyncRingBuffer<T>::Insert(T item)
{
	std::lock_guard<std::mutex> lock(m_lock);
	
	m_buffer.insert(m_buffer.begin() + m_head, item);
	//m_buffer[m_head] = item;

	//If I am full, reset the tail to be the first item
	if (m_isFull)
	{
		m_tail = (m_tail + 1) % m_maxSize;
	}

	m_head = (m_head + 1) % m_maxSize;

	m_isFull = (m_head == m_tail);
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
size_t UniformAsyncRingBuffer<T>::GetSize() const
{
	size_t size = m_maxSize;

	if (!m_isFull)
	{
		if (m_head >= m_tail)
		{
			size = m_head - m_tail;
		}
		else
		{
			size = m_maxSize + m_head - m_tail;
		}
	}

	return size;
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
size_t UniformAsyncRingBuffer<T>::GetCapacity() const
{
	return m_maxSize;
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
bool UniformAsyncRingBuffer<T>::IsFull() const
{
	if (m_tail = m_head + 1 && m_isFull)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
bool UniformAsyncRingBuffer<T>::IsEmpty() const
{
	//If head and tail are equal, we're empty
	if (!m_isFull && (m_head == m_tail))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
void UniformAsyncRingBuffer<T>::ResetBuffer()
{
	std::lock_guard<std::mutex> lock(m_lock);
	m_head = m_tail;
	m_isFull = false;
}

//------------------------------------------------------------------------------------------------------------------------------
template <class T>
UniformAsyncRingBuffer<T>::UniformAsyncRingBuffer(size_t bufferSize)
	: m_maxSize(bufferSize)
{
	m_buffer.reserve(bufferSize);
}
