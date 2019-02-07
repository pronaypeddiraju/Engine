//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------

UniformBuffer::UniformBuffer( RenderContext *owningRenderContext )
	: RenderBuffer(owningRenderContext)
{

}

UniformBuffer::~UniformBuffer()
{

}

bool UniformBuffer::CopyCPUToGPU( void const *data, size_t const byteSize )
{

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (byteSize > GetSize() || IsStatic()) 
	{
		RenderBuffer renderBuffer(m_owningRenderContext);
		bool result = renderBuffer.CreateBuffer( data, 
			byteSize,        // total size needed for buffer?
			byteSize,        // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_UNIFORM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC ); // probably want dynamic if we're using copy

		return result; 

	} else {
		// non-static and we have enough room, so call down 
		// to our base classes CopyGPUToGPU that assumes
		// enough room is already allocated; 
		ASSERT(IsDynamic());
		if (RenderBuffer::CopyCPUToGPU( data, byteSize )) {
			//m_vertexCount = count; 
			return true; 
		}
	}

	return false; 
}