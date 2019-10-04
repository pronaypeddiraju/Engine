#pragma once
#include "Engine/Math/Matrix44.hpp"
#include <string>

class RenderContext;
class Material;
class GPUMesh;

class Model
{
public:
	Model();
	Model(RenderContext *ctx, const std::string& meshName);
	Model(GPUMesh *mesh);
	~Model();

public:

	RenderContext* m_context;
	GPUMesh *m_mesh;
	Material *m_material;
	Matrix44 m_modelMatrix;
};