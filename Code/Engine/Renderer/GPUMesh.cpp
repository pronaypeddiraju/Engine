//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

//------------------------------------------------------------------------------------------------------------------------------
GPUMesh::GPUMesh( RenderContext *renderContext )
{
	m_vertexBuffer = new VertexBuffer(renderContext);
	m_indexBuffer = new IndexBuffer(renderContext);
}

//------------------------------------------------------------------------------------------------------------------------------
GPUMesh::~GPUMesh()
{
	if (m_vertexBuffer != nullptr)
	{
		delete m_vertexBuffer;
	}
	m_vertexBuffer = nullptr;

	if (m_indexBuffer != nullptr)
	{
		delete m_indexBuffer;
	}
	m_indexBuffer = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
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

void GPUMesh::CopyIndices(uint const *indices, uint count)
{
	bool result = m_indexBuffer->CreateStaticFor(indices, count);

	if (!result)
	{
		ASSERT_RECOVERABLE(true, "Index buffer not created")
	}

	SetDrawCall((count > 0), (count > 0) ? count : m_elementCount);
}

/*
void GPUMesh::CreateFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem /*= GPU_MEMORY_USAGE_STATIC  )
{
	/*
	// updated in A05 - NOT FINAL
	// This is now going to take a buffer layout and use that to generate the GPU Mesh

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
		pcu.m_position = mesh->m_vertices[vIdx].m_position; 
		pcu.m_color = mesh->m_vertices[vIdx].m_color; 
		pcu.m_uvTexCoords = mesh->m_vertices[vIdx].m_uv; 

		pcuVertices.push_back(pcu); 
	}

	m_vertexBuffer->CreateStaticFor( &pcuVertices[0], vcount);
	m_indexBuffer->CreateStaticFor( &mesh->m_indices[0], mesh->GetIndexCount() ); 

	SetDrawCall( mesh->UsesIndexBuffer(), mesh->GetElementCount() ); 

	//New Implementation A06
	const BufferLayout* layout = mesh->GetLayout();
	if(layout == nullptr)
	{
		ERROR_AND_DIE("The buffer layout recieved from the CPU Mesh was nullptr!");
	}

	//We actually have a buffer layout with valid data

	// 1. Need to allocate a buffer to copy into that matches our layout; 
	size_t sizeNeeded = layout->GetStride() * layout->GetAttributeCount();
	void* buffer = malloc(sizeNeeded);

	// 2. Copy into that buffer
	layout->m_copyFromMaster(buffer, mesh->GetVertices(), mesh->GetVertexCount());

	// 3. Update or create using this buffer
	if(mem != GPU_MEMORY_USAGE_STATIC)
	{
		ERROR_RECOVERABLE("Creating STATIC mesh from CPU but GPU mem type is not static");
	}

	m_vertexBuffer->CreateStaticFor( mesh->GetVertices(), m_layout->m_stride);

	



	// 4. Save off the layout - needed to create input layouts later; 
	m_layout = layout; 

	// don't forget to free memory allocated!!!
	free( buffer ); 

}


void GPUMesh::CopyFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem )
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
		pcu.m_position = mesh->m_vertices[vIdx].m_position; 
		pcu.m_color = mesh->m_vertices[vIdx].m_color; 
		pcu.m_uvTexCoords = mesh->m_vertices[vIdx].m_uv; 

		pcuVertices.push_back(pcu); 
	}

	m_vertexBuffer->CopyCPUToGPU( &pcuVertices[0], vcount);
	m_indexBuffer->CopyCPUToGPU( &mesh->m_indices[0], mesh->GetIndexCount() ); 

	SetDrawCall( mesh->UsesIndexBuffer(), mesh->GetElementCount() ); 
}
*/