#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/RenderBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------
struct Vertex_PCU;

//------------------------------------------------------------------------------------------------------------------------------
// VertexBuffer.hpp
//------------------------------------------------------------------------------------------------------------------------------

// Convenience class - makes working with render buffers meant for
// vertex buffers a little easier; 
class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer(RenderContext *owner );
	~VertexBuffer();

	bool CopyCPUToGPU( void const *vertices, uint const count, uint const stride ); 

	//For when we need a static vertex buffer (ex: static meshes)
	bool CreateStaticFor( Vertex_PCU const *vertices, uint const count );
	
	//Vertex agnostic
	bool CreateStaticForBuffer( void const *vertices, uint stride, uint const count );
	inline uint GetVertexCount() { return m_vertexCount; }

public:
	uint m_vertexCount = 0;                   // how many vertices are stored currently; 
										  // BufferLayout const *m_layout;   // TODO: VertexBuffer knows what kind of vertices are in it; 
};