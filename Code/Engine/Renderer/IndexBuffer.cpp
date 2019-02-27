#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

IndexBuffer::IndexBuffer( RenderContext *renderContext )
	: RenderBuffer(renderContext)
{

}

IndexBuffer::~IndexBuffer()
{

}

bool IndexBuffer::CreateStaticFor( uint const *indices, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(unsigned int); 

	// just always create for static a new static buffer; 
	bool result = CreateBuffer( indices, 
		sizeNeeded,        // total size needed for buffer?
		sizeof(unsigned int), // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_INDEX_STREAM_BIT, 
		GPU_MEMORY_USAGE_STATIC ); // probably want dynamic if we're using copy

	if (result) 
	{
		m_indexCount = count; 
		return true; 
	} 
	else 
	{
		m_indexCount = 0U; 
		return false; 
	}
}

bool IndexBuffer::CopyCPUToGPU( uint const *indices, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(Vertex_PCU); 

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded > GetSize() || IsStatic()) 
	{
		bool result = CreateBuffer( indices, 
			sizeNeeded,        // total size needed for buffer?
			sizeof(Vertex_PCU), // stride - size from one vertex to another
			RENDER_BUFFER_USAGE_INDEX_STREAM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC ); // probably want dynamic if we're using copy

		if (result) 
		{
			m_indexCount = count; 
		} 
		else 
		{
			m_indexCount = 0U; 
		}

		return result;
	} 
	else 
	{
		// non-static and we have enough room
		//GUARANTEE_RECOVERABLE( IsDynamic(), "The GPU Memory mode is not dynamic!" ); 
		ASSERT( IsDynamic() ); 
		if (RenderBuffer::CopyCPUToGPU( indices, sizeNeeded )) 
		{
			m_indexCount = count; 
			return true;
		}
	}

	return false;
}

