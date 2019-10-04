#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Allocators/InternalAllocator.hpp"
#include <mutex>

typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
struct Block_T
{
	Block_T* next;
};

//------------------------------------------------------------------------------------------------------------------------------
struct Chunck_T
{
	Chunck_T* next;
};

// Single-Threaded Block Allocator
class BlockAllocator : public InternalAllocator
{
public:
	//This Initialization takes a base allocator to sub allocate from
	//The allocation can grow as long as the base allocator can allocate
	bool						Initialize(InternalAllocator* base,
								size_t blockSize,
								size_t alignment,
								uint blocksPerChunk);

	//Takes a static buffer of fixed size. This allocation is not allowed to grow
	bool						Initialize(void* buffer,
								size_t bufferSize,
								size_t blockSize,
								size_t alignment);

	void						Deinitialize();

	//Interface methods
	virtual void*				Allocate(size_t size) final; // works as long as size <= block_size
	virtual void				Free(void* ptr) final;

	//------------------------------------------------------------------------------------------------------------------------------
	//Static methods
	static	BlockAllocator*		CreateInstance();
	static	void				DestroyInstance();
	static	BlockAllocator*		GetInstance();

private:

	//Block allocator can allocate and free single blocks
	void*						AllocateBlock();
	void						FreeBlock(void* ptr);

	//Allocated a chunk and splits into blocks
	//NOTE: Will fail if there is no base allocator provided
	bool						AllocateChunk();
	void						BreakUpChunk(void* buffer);

	void						PushFreeBlock(Block_T* block);
	Block_T*					PopFreeBlock();

private: 

	//Sub allocator to use for allocation
	InternalAllocator*			m_base = nullptr;

	Block_T*					m_freeBlocks = nullptr;
	Chunck_T*					m_chunkList = nullptr;  

	size_t						m_alignment;
	size_t						m_blockSize;
	size_t						m_blocksPerChunk;

	size_t						m_bufferSize;

	// AsyncBlockAllocator
	std::mutex					m_chunkLock;
	std::mutex					m_blockLock;
};
