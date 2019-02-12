//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include <d3d11.h>

class RenderContext;

struct ID3D11Buffer;

//------------------------------------------------------------------------
// RenderBuffer.hpp
// Render Buffers are GPU storage for information.  
//------------------------------------------------------------------------

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

	static D3D11_USAGE DXUsageFromMemoryUsage( eGPUMemoryUsage const usage );

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

	size_t						m_bufferSize = 0;    // total byte count of this buffer; 
	size_t						m_elementSize = 0;   // size of the stride for each thing in buffer

	// D3D11 Resource
	ID3D11Buffer*				m_handle = nullptr;  
}; 