#pragma once
#include <stdint.h>
#include <mutex>

//------------------------------------------------------------------------------------------------------------------------------
typedef uint8_t byte;
typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
// Look up bit fields in C. This is basically creating RinBufferMeta_T of size uint where the first 31 bits are size and 
// the last bit is to tell if the bufferObject is unlocked.
//------------------------------------------------------------------------------------------------------------------------------
struct RingBufferMeta_T
{
	uint bufferObjectSize : 31;
	uint isBufferObjectUnlocked : 1;
};

//------------------------------------------------------------------------------------------------------------------------------
// This is a Multiple Producer Single Consumer Async Ring Buffer. 
//------------------------------------------------------------------------------------------------------------------------------
class MPSCRingBuffer
{
	public:
		MPSCRingBuffer();
		~MPSCRingBuffer();

		bool			InitializeBuffer(size_t sizeInBytes);
		void			ReleaseBuffer();    // called from de-constructor

		//Instantly returns, either receive valid data pointer or a nullptr
		void*			TryLockWrite(size_t size);
		//Block until there room to write onto buffer
		void*			LockWrite(size_t size);
		void			UnlockWrite(void* ptr);

		size_t			GetWritableSpace() const;
		
		void*			TryLockRead(size_t* outSize);
		void*			LockRead(size_t* outSize);
		void			UnlockRead(void* ptr);

		// helpers - optional
		bool write(void const* data, size_t byte_size);
		bool try_write(void const* data, size_t byte_size);

		//If fail, return 0. Else, return the number of bytes read
		size_t			Read(void* outData);
		size_t			TryRead(void* outData);

	private:
		byte*			m_buffer = nullptr;
		size_t			m_byteSize = 0;

		uint			m_writeHead = 0;
		uint			m_readHead = 0;

		std::mutex		m_lock;
};