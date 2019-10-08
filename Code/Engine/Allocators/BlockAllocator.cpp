#include "Engine/Allocators/BlockAllocator.hpp"
#include "Engine/Core/MemTracking.hpp"

//------------------------------------------------------------------------------------------------------------------------------
typedef uint8_t byte;
BlockAllocator* gBlockAllocator = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
bool BlockAllocator::Initialize(InternalAllocator* base, size_t blockSize, size_t alignment, uint blocksPerChunk)
{
	m_base = base;
	m_blockSize = blockSize;
	m_alignment = alignment;
	m_blocksPerChunk = blocksPerChunk;

	m_freeBlocks = nullptr;
	m_chunkList = nullptr;

	AllocateChunk();  

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool BlockAllocator::Initialize(void* buffer, size_t bufferSize, size_t blockSize, size_t alignment)
{
	// infer class members based on parameters
	m_blocksPerChunk = bufferSize/ blockSize;
	m_bufferSize = bufferSize;
	m_blockSize = blockSize;
	m_alignment = alignment;

	m_base = nullptr;
	m_freeBlocks = nullptr;

	// allocating blocks from a chunk
	// may move this to a different method later; 
	BreakUpChunk(buffer);

	if (m_freeBlocks != nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BlockAllocator::Deinitialize()
{
	//If m_base is not nullptr, we have chunks
	if (m_base) 
	{
		std::scoped_lock chunkLock(m_chunkLock);

		Chunck_T* list = nullptr;

		// free chunks
		while (m_chunkList != nullptr) 
		{
			list = m_chunkList;
			m_chunkList = m_chunkList->next;
			m_base->Free(list);
		}
	} //Else condition is normal cleanup

	std::scoped_lock blockLock(m_blockLock);

	//Reset members
	m_base = nullptr;
	m_freeBlocks = nullptr;
	m_blockSize = 0U;
	m_blocksPerChunk = 0U;
}

//------------------------------------------------------------------------------------------------------------------------------
void* BlockAllocator::Allocate(size_t size)
{
	if (size <= m_blockSize)
	{
		return AllocateBlock();
	}
	else
	{
		return nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BlockAllocator::Free(void* ptr)
{
	FreeBlock(ptr);
}

//------------------------------------------------------------------------------------------------------------------------------
void* BlockAllocator::AllocateBlock()
{
	Block_T* block = PopFreeBlock();
	
	while (block == nullptr) 
	{
		if (!AllocateChunk()) 
		{
			return nullptr;
		}

		block = PopFreeBlock();
	}

	return block;
}

//------------------------------------------------------------------------------------------------------------------------------
void BlockAllocator::FreeBlock(void* ptr)
{
	Block_T* block = (Block_T*)ptr;
	PushFreeBlock(block);
}

//------------------------------------------------------------------------------------------------------------------------------
BlockAllocator* BlockAllocator::CreateInstance()
{
	if (gBlockAllocator == nullptr)
	{
		gBlockAllocator = new BlockAllocator();
	}

	return gBlockAllocator;
}

//------------------------------------------------------------------------------------------------------------------------------
void BlockAllocator::DestroyInstance()
{
	if (gBlockAllocator != nullptr)
	{
		delete gBlockAllocator;
		gBlockAllocator = nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
BlockAllocator* BlockAllocator::GetInstance()
{
	if (gBlockAllocator == nullptr)
	{
		CreateInstance();
	}

	return gBlockAllocator;
}

//------------------------------------------------------------------------------------------------------------------------------
bool BlockAllocator::AllocateChunk()
{
	//We can't allocate a chunk if we don't have a sub allocator
	if (m_base == nullptr) 
	{
		return false;
	}

	if (m_chunkLock.try_lock()) 
	{
		//Allocate a chunk of memory if the base allocator is able to 
		size_t chunkSize = m_blocksPerChunk * m_blockSize + sizeof(Block_T);

		Chunck_T* chunk = (Chunck_T*)m_base->Allocate(chunkSize);
		if (chunk == nullptr) 
		{
			return false;
		}

		//Track this chunk so we can free this later
		chunk->next = m_chunkList;
		m_chunkList = chunk;

		//Break up newly allocated chunk
		BreakUpChunk(chunk + 1);
		m_chunkLock.unlock();
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void BlockAllocator::BreakUpChunk(void* buffer)
{
	byte* buf = (byte*)buffer;
	Block_T* first = (Block_T*)buf;
	Block_T* head = nullptr;

	for (uint i = 0; i < m_blocksPerChunk; ++i) 
	{
		Block_T* node = (Block_T*)buf;
		buf += m_blockSize;

		node->next = head;
		head = node;
	}

	{
		//Lock so we are thread safe
		std::scoped_lock lock(m_blockLock);
		
		first->next = m_freeBlocks;
		m_freeBlocks = head;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BlockAllocator::PushFreeBlock(Block_T* block)
{
	std::scoped_lock blockLock(m_blockLock);

	block->next = m_freeBlocks;
	m_freeBlocks = block;
}

//------------------------------------------------------------------------------------------------------------------------------
Block_T* BlockAllocator::PopFreeBlock()
{
	std::scoped_lock blockLock(m_blockLock);

	Block_T* head = m_freeBlocks;
	if (head != nullptr) 
	{
		m_freeBlocks = head->next;
	}

	return head;
}

