//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class CPUMesh;
class IndexBuffer;
struct Vertex_PCU;

//------------------------------------------------------------------------------------------------------------------------------
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

	inline bool				UsesIndexBuffer() {return m_useIndexBuffer;}
	inline uint				GetElementCount() {return m_elementCount;}
	inline uint				GetVertexCount() {return m_vertexBuffer->GetVertexCount();}

public: 
	VertexBuffer*			m_vertexBuffer = nullptr; 
	IndexBuffer*			m_indexBuffer = nullptr; 

	// information for drawing; 
	uint					m_elementCount = 0U; 
	bool					m_useIndexBuffer; 
};
