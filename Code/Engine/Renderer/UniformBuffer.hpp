#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/RenderBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------
// convenience class - makes working with a RenderBuffer
// for the purposes of constant data a little easier; 
class UniformBuffer : public RenderBuffer
{
public:
	UniformBuffer( RenderContext *owner ); 
	~UniformBuffer(); 

	// UniformBuffer's is public, as we can now make some assumptions
	// (for example, UniformBuffer's are usually dynamic based on our usage
	// (they don't have to be, but they are usually used for data that changes
	// so we'll have the default behaviour do that).
	bool CopyCPUToGPU( void const *data, size_t const byteSize ); 
}; 