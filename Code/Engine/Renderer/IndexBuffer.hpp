#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"
//------------------------------------------------------------------------

class IndexBuffer : public RenderBuffer        // A04
{
public:
	IndexBuffer( RenderContext *renderContext);       // A04
	~IndexBuffer();

											 // Technically you can create 16-bit or 32-bit index buffers
											 // but for simplicity we'll stick to just 32-bit; 
	bool CreateStaticFor( uint const *indices, uint const count );          // A04
	bool CopyCPUToGPU( uint const *indices, uint const count );            // A04

public: 
	uint m_indexCount;                      // A04
};

