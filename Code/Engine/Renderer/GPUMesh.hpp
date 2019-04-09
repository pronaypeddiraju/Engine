//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class IndexBuffer;
struct Vertex_PCU;

//------------------------------------------------------------------------------------------------------------------------------
class GPUMesh                            // A04
{
	friend class CPUMesh;

public:
	GPUMesh( RenderContext *renderContext );     // A04
	~GPUMesh();

	// these actually mostly do the same thing - just differnt default
	// memory usage
	template <typename VertexType>
	void					CreateFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem = GPU_MEMORY_USAGE_STATIC ); // A04

	template <typename VertexType>
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
	BufferLayout*			m_layout = nullptr;

	// information for drawing; 
	uint					m_elementCount = 0U; 
	bool					m_useIndexBuffer; 
};

//------------------------------------------------------------------------------------------------------------------------------
template <typename VertexType>
void GPUMesh::CreateFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem )
{
	//New Implementation A06
	const BufferLayout* layout = VertexType::layout;
	if(layout == nullptr)
	{
		ERROR_AND_DIE("The buffer layout recieved from the CPU Mesh was nullptr!");
	}

	if(mem != GPU_MEMORY_USAGE_STATIC)
	{
		ERROR_RECOVERABLE("Creating STATIC mesh from CPU but GPU mem type is not static");
	}
	//We actually have a buffer layout with valid data
	std::vector<VertexType> vertices;

	uint vcount = mesh->GetVertexCount(); 
	vertices.reserve( vcount ); 

	layout->m_copyFromMaster(vertices.data(), mesh->GetVertices(), vcount);

	/*
	for ( uint vIdx = 0; vIdx < vcount; ++vIdx ) 
	{
		VertexType vert(mesh->m_vertices[vIdx]); 

		
		vertices.push_back(vert); 
	}
	*/

	m_vertexBuffer->CreateStaticForBuffer(vertices.data(), layout->m_stride, vcount);
	m_indexBuffer->CreateStaticFor( mesh->GetIndices(), mesh->GetIndexCount() ); 

	SetDrawCall( mesh->UsesIndexBuffer(), mesh->GetElementCount() ); 

	m_layout = (BufferLayout*)layout;
}

//------------------------------------------------------------------------------------------------------------------------------
 template <typename VertexType>
void GPUMesh::CopyFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem )
{
	const BufferLayout* layout = VertexType::layout;
	if(layout == nullptr)
	{
		ERROR_AND_DIE("The buffer layout recieved from the CPU Mesh was nullptr!");
	}

	if(mem != GPU_MEMORY_USAGE_DYNAMIC)
	{
		ERROR_RECOVERABLE("Creating DYNAMIC mesh from CPU but GPU mem type is not Dynamic");
	}

	std::vector<VertexType> vertices; 

	uint vcount = mesh->GetVertexCount(); 
	vertices.reserve( vcount ); 

	layout->m_copyFromMaster(vertices.data(), mesh->GetVertices(), vcount);

	m_vertexBuffer->CopyCPUToGPU( vertices.data(), vcount, layout->m_stride);
	m_indexBuffer->CopyCPUToGPU( mesh->GetIndices(), mesh->GetIndexCount() ); 

	SetDrawCall( mesh->UsesIndexBuffer(), mesh->GetElementCount() ); 
	m_layout = layout;
}