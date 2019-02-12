//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Vertex_PCU.hpp"

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------

VertexBuffer::VertexBuffer(RenderContext *owner )
	: RenderBuffer(owner )
{

}

VertexBuffer::~VertexBuffer()
{

}

// Similar to UBO - since we are assuming if they are using this method 
// it is dynamic, and we only need to remake if the size changes; 
bool VertexBuffer::CopyCPUToGPU( Vertex_PCU const *vertices, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(Vertex_PCU); 

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded > GetSize() || IsStatic()) 
	{
		bool result = CreateBuffer( vertices, 
			sizeNeeded,        // total size needed for buffer?
			sizeof(Vertex_PCU), // stride - size from one vertex to another
			RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC ); // probably want dynamic if we're using copy

		if (result) 
		{
			m_vertexCount = count; 
		} 
		else 
		{
			m_vertexCount = 0U; 
		}

		return result;
	} 
	else 
	{
		// non-static and we have enough room
		//GUARANTEE_RECOVERABLE( IsDynamic(), "The GPU Memory mode is not dynamic!" ); 
		ASSERT( IsDynamic() ); 
		if (RenderBuffer::CopyCPUToGPU( vertices, sizeNeeded )) 
		{
			m_vertexCount = count; 
			return true;
		}
	}

	return false;
}

//Creating a static buffer because we could store a static mesh in which case the vertices dont 
//change and can be saved as a static buffer
bool VertexBuffer::CreateStaticFor( Vertex_PCU const *vertices, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(Vertex_PCU); 

	// just always create for static a new static buffer; 
	bool result = CreateBuffer( vertices, 
		sizeNeeded,        // total size needed for buffer?
		sizeof(Vertex_PCU), // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT, 
		GPU_MEMORY_USAGE_STATIC ); // probably want dynamic if we're using copy

	if (result) 
	{
		m_vertexCount = count; 
		return true; 
	} 
	else 
	{
		m_vertexCount = 0U; 
		return false; 
	}
}