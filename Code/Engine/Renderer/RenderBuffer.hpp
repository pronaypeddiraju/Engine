#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

struct ID3D11Buffer;

//------------------------------------------------------------------------
// RenderBuffer.hpp
// Render Buffers are GPU storage for information.  
//------------------------------------------------------------------------

// We have to specify what can a buffer be used 
// for.  It is possible for a buffer to serve multiple purposes
// though for our case, we'll be special casing them for clarity; 
enum eRenderBufferUsageBit : uint
{
	RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT   = BIT_FLAG(0),   // Can be bound to an vertex input stream slot
	RENDER_BUFFER_USAGE_INDEX_STREAM_BIT    = BIT_FLAG(1),   // Can be bound as an index input stream.  
	RENDER_BUFFER_USAGE_UNIFORM_BIT        = BIT_FLAG(2),   // Can be bound to a constant buffer slot; 
};
typedef uint eRenderBufferUsageBits; 

// In D3D11 and GL - buffers also are supplied hints on 
// desired usage patterns to help the GPU optimize for it
enum eGPUMemoryUsage 
{
	GPU_MEMORY_USAGE_GPU,     // Can be written/read from GPU only (Color Targets are a good example)
	GPU_MEMORY_USAGE_STATIC,  // Created, and are read only after that (ex: textures from images, sprite atlas)
	GPU_MEMORY_USAGE_DYNAMIC, // Update often from CPU, used by the GPU (CPU -> GPU updates, used by shaders.  ex: Uniform Buffers)
	GPU_MEMORY_USAGE_STAGING, // For getting memory from GPU to CPU (can be copied into, but not directly bound as an output.  ex: Screen shots system)
}; 

class RenderBuffer
{
	friend class UniformBuffer;

public:
	// for convenience - we'll have the RenderBuffer hold onto
	// the RenderContext used to create it so we're not constantly passing it in; 
	RenderBuffer( RenderContext *owner );
	~RenderBuffer(); 
	 
	size_t					GetSize() const;    // return max byte size of this buffer; 
	bool					IsStatic() const;     // has static usage?
	bool					IsDynamic() const; 

protected:
	// for doing initial setup - we'll mark 
	// it as protected as the higher level classes
	// will help limit the number of options; 
	bool					CreateBuffer( void const *initialData, 
								size_t bufferSize, 
								size_t elementSize, 
								eRenderBufferUsageBits usage, 
								eGPUMemoryUsage memUsage );

	// Assumes a buffer is already created, and copies this data into it; 
	// Only valid for DYNAMIC buffers; 
	bool					CopyCPUToGPU( void const *data, size_t const byteSize ); 

public:
	RenderContext*				m_owningRenderContext = nullptr; 
	eRenderBufferUsageBits		m_bufferUsage; 
	eGPUMemoryUsage				m_memoryUsage; 

	size_t						m_bufferSize;    // total byte count of this buffer; 
	size_t						m_elementSize;   // size of the stride for each thing in buffer

	// D3D11 Resource
	ID3D11Buffer*				m_handle = nullptr;  
}; 