//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Model.hpp"
//Engine Systems
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Model::Model(RenderContext *ctx, const std::string& meshName)
{
	m_context = ctx;
	m_mesh = ctx->CreateOrGetMeshFromFile(meshName);

	if (m_mesh != nullptr) {
		m_material = ctx->CreateOrGetMaterialFromFile(m_mesh->GetDefaultMaterialName());
	}

	m_modelMatrix = Matrix44::IDENTITY;
}

//------------------------------------------------------------------------------------------------------------------------------
Model::Model(GPUMesh *mesh)
{	
	m_mesh = mesh;
}

//------------------------------------------------------------------------------------------------------------------------------
Model::Model()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Model::~Model()
{

}

