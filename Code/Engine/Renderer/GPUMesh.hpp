//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/RenderContext.hpp"

class CPUMesh;
struct Vertex_PCU;
class VertexBuffer;
class IndexBuffer;

class GPUMesh                            // A04
{
public:
	GPUMesh( RenderContext *renderContext );     // A04
	~GPUMesh();

	// these actually mostly do the same thing - just differnt default
	// memory usage
	void					CreateFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem = GPU_MEMORY_USAGE_STATIC ); // A04
	void					CopyFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem = GPU_MEMORY_USAGE_DYNAMIC );  // A04

	void					CopyVertexArray( Vertex_PCU const *vertices, uint count );                           // A04 Optional; 
	void					CopyIndices( uint const *indices, uint count );                                      // A04 Optional;

	void					SetDrawCall( bool useIndexBuffer, uint elemCount ); 

public: 
	VertexBuffer*			m_vertexBuffer = nullptr; 
	IndexBuffer*			m_indexBuffer = nullptr; 

	// information for drawing; 
	uint					m_elementCount = 0U; 
	bool					m_useIndexBuffer; 
};
