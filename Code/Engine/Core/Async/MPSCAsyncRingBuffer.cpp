#include "Engine/Core/Async/MPSCAsyncRingBuffer.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"

//------------------------------------------------------------------------------------------------------------------------------
MPSCRingBuffer::MPSCRingBuffer()
{
	//Do nothing, the user must call Initialize to use Ring buffer
}

//------------------------------------------------------------------------------------------------------------------------------
MPSCRingBuffer::~MPSCRingBuffer()
{
	ReleaseBuffer();
}

//------------------------------------------------------------------------------------------------------------------------------
bool MPSCRingBuffer::InitializeBuffer(size_t sizeInBytes)
{
	if (m_buffer == nullptr)
	{
		m_buffer = (byte*)malloc(sizeInBytes);
		m_byteSize = sizeInBytes;
		return true;
	}
	else
	{
		ReleaseBuffer();
		return InitializeBuffer(sizeInBytes);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void MPSCRingBuffer::ReleaseBuffer()
{
	if (m_buffer != nullptr) 
	{
		free(m_buffer);
		m_buffer = nullptr;
		m_byteSize = 0U;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void* MPSCRingBuffer::TryLockWrite(size_t writeSize)
{
	ASSERT_OR_DIE(writeSize < (1 << 31), "The size of object to write exceeded the total size of the MPSC ring buffer");

	size_t metaDataSize = sizeof(RingBufferMeta_T);
	size_t totalSize = 2 * metaDataSize + writeSize;

	//If the space available is less than the size of meta data + write data, return nullptr
	std::scoped_lock lock(m_lock);
	if (GetWritableSpace() < totalSize) 
	{
		return nullptr;
	}

	//We have enough space in the buffer so let's write meta and alloc the required space
	uint newHead = (uint)(m_writeHead + totalSize);

	if (newHead > m_byteSize) 
	{
		//Buffer needs to wrap, so let's write a skip meta buffer so the read head will wrap at this point
		RingBufferMeta_T* skipBufferEntry = (RingBufferMeta_T*)(m_buffer + newHead);
		skipBufferEntry->bufferObjectSize = 0;  // 0 means skip; 
		skipBufferEntry->isBufferObjectUnlocked = 1;

		m_writeHead = 0;
	}

	//ASfter we wrapped we would like to check if the buffer has space again
	if (GetWritableSpace() < totalSize) 
	{
		return nullptr;
	}

	//My write usable buffer
	byte* usableBuffer = m_buffer + m_writeHead;

	RingBufferMeta_T* head = (RingBufferMeta_T*)usableBuffer;
	head->bufferObjectSize = (uint)writeSize;
	head->isBufferObjectUnlocked = 0;

	//uint usedHead = m_writeHead;
	m_writeHead += (int)totalSize;
	//we moved by 2 meta data in size so let's get back by 1 meta data size (TotalSize is 2*metaDataSize + writeSize)
	m_writeHead -= (int)metaDataSize;

	return head + 1;
}

//------------------------------------------------------------------------------------------------------------------------------
// No matter how long it takes, this call will lock the thread and make it wait for a write operation
//------------------------------------------------------------------------------------------------------------------------------
void* MPSCRingBuffer::LockWrite(size_t size)
{
	void* ptr = TryLockWrite(size);
	while (ptr == nullptr) 
	{
		std::this_thread::yield();
		ptr = TryLockWrite(size);
	}

	return ptr;
}

//------------------------------------------------------------------------------------------------------------------------------
size_t MPSCRingBuffer::GetWritableSpace() const
{
	// size_t remaining = (m_read_head - m_write_head - 1) % m_size;  
	size_t remaining = 0;
	if (m_writeHead >= m_readHead) 
	{
		remaining = m_byteSize - m_writeHead;
		remaining += m_readHead;
	}
	else 
	{
		remaining = m_readHead - m_writeHead;
	}

	return remaining;
}

//------------------------------------------------------------------------------------------------------------------------------
void MPSCRingBuffer::UnlockWrite(void* ptr)
{
	RingBufferMeta_T* writeHead = (RingBufferMeta_T*)ptr;
	--writeHead;

	writeHead->isBufferObjectUnlocked = 1;
}

//------------------------------------------------------------------------------------------------------------------------------
void* MPSCRingBuffer::TryLockRead(size_t* outSize)
{
	std::scoped_lock lock(m_lock);

	while (true) 
	{
		//If the buffer is empty return
		if (m_readHead == m_writeHead) 
		{
			return nullptr;
		}

		//Cast to Meta object and figure how big the buffer is to return it
		RingBufferMeta_T* readMeta = (RingBufferMeta_T*)(m_buffer + m_readHead);
		if (readMeta->isBufferObjectUnlocked) 
		{
			if (readMeta->bufferObjectSize == 0) 
			{
				// Wrap around case
				m_readHead = 0;
			}
			else 
			{
				// valid read case
				*outSize = readMeta->bufferObjectSize;

				// SINGLE CONSUMER CASE - nothing else happens
				void* returnBuffer = readMeta + 1;
				return returnBuffer;
			}
		}
		else 
		{
			// only one consumer - this shouldn't happen
			return nullptr;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void* MPSCRingBuffer::LockRead(size_t* outSize)
{
	void* ptr = TryLockRead(outSize);
	while (ptr == nullptr)
	{
		std::this_thread::yield();
		ptr = TryLockRead(outSize);
	}

	return ptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void MPSCRingBuffer::UnlockRead(void* ptr)
{
	std::scoped_lock lock(m_lock);

	RingBufferMeta_T* readHead = (RingBufferMeta_T*)ptr;
	readHead--;

	ASSERT_RECOVERABLE(((m_buffer + m_readHead) == (byte*)readHead), "The read head for MPSC Async Ring Buffer is invalid");

	m_readHead += sizeof(RingBufferMeta_T) + readHead->bufferObjectSize;
}
