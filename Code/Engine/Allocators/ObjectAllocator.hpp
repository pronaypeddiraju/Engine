#pragma once

//------------------------------------------------------------------------------------------------------------------------------
template <typename OBJ>
class ObjectAllocator : private BlockAllocator
{
public:
	void Initialize(InternalAllocator* parent, uint blocksPerChunk)
	{
		return BlockAllocator::Initialize(parent, sizeof(OBJ), alignof(OBJ), blocksPerChunk);
	}

	void Deinitialize()
	{
		BlockAllocator::Deinitialize();
	}

	virtual void* Allocate(size_t size)
	{
		return BlockAllocator::Allocate(size);
	}

	virtual void* Free(void* ptr)
	{
		return BlockAllocator::Free(ptr);
	}

	template <typename ...ARGS>
	OBJ* Create(ARGS ...args)
	{
		void* mem = Allocate(sizeof(OBJ));
		if (mem != nullptr)
		{
			return new(mem) OBJ(std::forward(args)...);
		}
		else
		{
			return nullptr;
		}
	}

	void Destroy(OBJ* object)
	{
		Free(object);
	}
};