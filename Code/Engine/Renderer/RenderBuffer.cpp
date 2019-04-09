//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// D3D11 STUFF
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

//------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------
// RenderBuffer.cpp
//------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------
// translate external enum to D3D11 specific options; 
// These will be used by texture as well,
// so may want to move this function to a common include; 
//------------------------------------------------------------------------------------------------------------------------------
STATIC D3D11_USAGE RenderBuffer::DXUsageFromMemoryUsage( eGPUMemoryUsage const usage )
{
	switch (usage) 
	{
		case GPU_MEMORY_USAGE_GPU:      return D3D11_USAGE_DEFAULT;
		case GPU_MEMORY_USAGE_STATIC:   return D3D11_USAGE_IMMUTABLE;
		case GPU_MEMORY_USAGE_DYNAMIC:  return D3D11_USAGE_DYNAMIC;
		case GPU_MEMORY_USAGE_STAGING:  return D3D11_USAGE_STAGING;
		default:
		{
			GUARANTEE_RECOVERABLE(false, "Setting D3D11 usage to dynamic"); 
			return D3D11_USAGE_DYNAMIC;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
// Convert a buffer usage bitfield to a DX specific 
// version; 
//------------------------------------------------------------------------------------------------------------------------------
static uint DXBufferUsageFromBufferUsage( eRenderBufferUsageBits const usage )
{
	uint ret = 0U;

	if (usage & RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT) {
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (usage & RENDER_BUFFER_USAGE_INDEX_STREAM_BIT) {
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if (usage & RENDER_BUFFER_USAGE_UNIFORM_BIT) {
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

//------------------------------------------------------------------------------------------------------------------------------
// Creates a buffer; 
//------------------------------------------------------------------------------------------------------------------------------
bool RenderBuffer::CreateBuffer( void const *initialData, 
	size_t bufferSize, 
	size_t elementSize, 
	eRenderBufferUsageBits bufferUsage, 
	eGPUMemoryUsage memUsage )
{
	// Free the old handle
	DX_SAFE_RELEASE(m_handle); 

	// can not create a 0 sized buffer; 
	ASSERT_RETURN_VALUE( (bufferSize > 0U) && (elementSize > 0U), false );

	// static buffers MUST be supplied data.
	ASSERT_RETURN_VALUE( (memUsage != GPU_MEMORY_USAGE_STATIC) || (initialData != nullptr), false );

	// Setup the buffer
	// When creating a D3D object, we setup
	// a DESC object to describe it...
	D3D11_BUFFER_DESC bufferDesc;
	memset( &bufferDesc, 0, sizeof(bufferDesc) );

	bufferDesc.ByteWidth = (UINT) bufferSize;
	bufferDesc.StructureByteStride = (UINT) elementSize;
	bufferDesc.Usage = DXUsageFromMemoryUsage(memUsage);
	bufferDesc.BindFlags = DXBufferUsageFromBufferUsage(bufferUsage);

	// give us write access to dynamic buffers
	// and read/write access to staging buffers; 
	bufferDesc.CPUAccessFlags = 0U;
	if (memUsage == GPU_MEMORY_USAGE_DYNAMIC) 
	{
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (memUsage == GPU_MEMORY_USAGE_STAGING) 
	{
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	// TODO: support structured buffers (Summer Semester)
	//I will probably forget about this....

	// Map initial data if relevant (something was passed in)
	D3D11_SUBRESOURCE_DATA data;
	D3D11_SUBRESOURCE_DATA *data_ptr = nullptr;
	if (initialData != nullptr) 
	{
		memset( &data, 0, sizeof(data) );
		data.pSysMem = initialData;
		data_ptr = &data;
	}

	// Create it - devices create resources; 
	ID3D11Device *dev = m_owningRenderContext->m_D3DDevice; 
	HRESULT hr = dev->CreateBuffer( &bufferDesc, data_ptr, &m_handle );

	if (SUCCEEDED(hr)) 
	{
		// save off options; 
		m_bufferUsage = bufferUsage;
		m_memoryUsage = memUsage; 
		m_bufferSize = bufferSize; 
		m_elementSize = elementSize; 
		return true;
	}
	else 
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool RenderBuffer::CopyCPUToGPU( void const *data, size_t const byteSize )
{
	// staging or dynamic only & we better have room; 
	ASSERT( !IsStatic() ); 
	ASSERT( byteSize <= m_bufferSize ); 
	
	// Map and copy
	// This is a command, so runs using the context
	ID3D11DeviceContext *deviceContext = m_owningRenderContext->m_D3DContext; 

	// Map (ie, lock and get a writable pointer)
	// Be sure to ONLY write to what you locked
	D3D11_MAPPED_SUBRESOURCE resource; 
	HRESULT hr = deviceContext->Map( m_handle, 
		0,    // resource index (for arrays/mip layers/etc...)
		D3D11_MAP_WRITE_DISCARD,  // says to discard (don't care) about the memory the was already there
		0U,   // option to allow this to fail if the resource is in use, 0U means we'll wait...
		&resource ); 

	if (SUCCEEDED(hr)) 
	{
		// we're mapped!  Copy over
		memcpy( resource.pData, data, byteSize ); 

		// unlock the resource (we're done writing)
		deviceContext->Unmap( m_handle, 0 ); 

		return true; 

	} 
	else 
	{
		return false; 
	}
}

//------------------------------------------------------------------------------------------------------------------------------
RenderBuffer::RenderBuffer( RenderContext *owner )
{
	m_owningRenderContext = owner;
}

//------------------------------------------------------------------------------------------------------------------------------
RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);

	m_owningRenderContext = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
size_t RenderBuffer::GetSize() const
{
	return m_bufferSize;
}

//------------------------------------------------------------------------------------------------------------------------------
bool RenderBuffer::IsStatic() const
{
	if(m_memoryUsage == GPU_MEMORY_USAGE_STATIC)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool RenderBuffer::IsDynamic() const
{
	if(m_memoryUsage == GPU_MEMORY_USAGE_DYNAMIC)
	{
		return true;
	}
	else
	{
		return false;
	}
}