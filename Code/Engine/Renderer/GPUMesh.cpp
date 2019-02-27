//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

GPUMesh::GPUMesh( RenderContext *renderContext )
{
	m_vertexBuffer = new VertexBuffer(renderContext);
	m_indexBuffer = new IndexBuffer(renderContext);
}

GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

void GPUMesh::SetDrawCall( bool useIndexBuffer, uint elemCount )
{
	if (useIndexBuffer) {
		ASSERT_RECOVERABLE( (m_indexBuffer->GetIndexCount() <= elemCount), "The index count is larger than the number of elements" ); 
	} else {
		ASSERT_RECOVERABLE( (m_vertexBuffer->GetVertexCount() <= elemCount), "The vertex count is larger than the number of elements" ); 
	}

	m_elementCount = elemCount; 
	m_useIndexBuffer = useIndexBuffer; 
}

void GPUMesh::CreateFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem /*= GPU_MEMORY_USAGE_STATIC */ )
{
	// updated in A05 - NOT FINAL

	if(mem != GPU_MEMORY_USAGE_STATIC)
	{
		ERROR_RECOVERABLE("Creating STATIC mesh from CPU but GPU mem type is not static");
	}

	std::vector<Vertex_PCU> pcuVertices; 

	uint vcount = mesh->GetVertexCount(); 
	pcuVertices.reserve( vcount ); 

	for ( uint vIdx = 0; vIdx < vcount; ++vIdx ) 
	{
		Vertex_PCU pcu; 

		// copy from master;
		pcu.m_position = mesh->m_vertices[vIdx].position; 
		pcu.m_color = mesh->m_vertices[vIdx].color; 
		pcu.m_uvTexCoords = mesh->m_vertices[vIdx].uv; 

		pcuVertices.push_back(pcu); 
	}

	m_vertexBuffer->CreateStaticFor( &pcuVertices[0], vcount);
	m_indexBuffer->CreateStaticFor( &mesh->m_indices[0], mesh->GetIndexCount() ); 

	SetDrawCall( mesh->UsesIndexBuffer(), mesh->GetElementCount() ); 
}

void GPUMesh::CopyFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem /*= GPU_MEMORY_USAGE_DYNAMIC */ )
{
	if(mem != GPU_MEMORY_USAGE_DYNAMIC)
	{
		ERROR_RECOVERABLE("Creating DYNAMIC mesh from CPU but GPU mem type is not Dynamic");
	}

	std::vector<Vertex_PCU> pcuVertices; 

	uint vcount = mesh->GetVertexCount(); 
	pcuVertices.reserve( vcount ); 

	for ( uint vIdx = 0; vIdx < vcount; ++vIdx ) 
	{
		Vertex_PCU pcu; 

		// copy from master;
		pcu.m_position = mesh->m_vertices[vIdx].position; 
		pcu.m_color = mesh->m_vertices[vIdx].color; 
		pcu.m_uvTexCoords = mesh->m_vertices[vIdx].uv; 

		pcuVertices.push_back(pcu); 
	}

	m_vertexBuffer->CopyCPUToGPU( &pcuVertices[0], vcount);
	m_indexBuffer->CopyCPUToGPU( &mesh->m_indices[0], mesh->GetIndexCount() ); 

	SetDrawCall( mesh->UsesIndexBuffer(), mesh->GetElementCount() ); 
}
