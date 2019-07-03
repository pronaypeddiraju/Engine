//------------------------------------------------------------------------------------------------------------------------------
#pragma once
// Engine Systems
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
// Others
#include <string>
#include <vector>

class CPUMesh;
class GPUMesh;
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
struct ObjIndex
{
	int vertexIndex;
	int uvIndex;
	int normalIndex;
};

//------------------------------------------------------------------------------------------------------------------------------
class ObjectLoader
{
public:
	ObjectLoader();

	static ObjectLoader*	CreateMeshFromFile(RenderContext* renderContext, const std::string& filePath, bool isDataDriven);

	void					LoadFromXML(const std::string& fileName);
	void					CreateFromString(const char* data);
	void					AddIndexForMesh(const std::string& indices);
	void					CreateCPUMesh();
	void					CreateGPUMesh();
public:
	std::vector<Vec3>				m_positions;
	std::vector<Vec2>				m_uvs;
	std::vector<Vec3>				m_normals;
	std::vector<ObjIndex>			m_indices;
	
	RenderContext*					m_renderContext = nullptr;
	CPUMesh*						m_cpuMesh = nullptr;
	GPUMesh*						m_mesh = nullptr;

	std::string						m_source = "";
	std::string						m_transform = "";
	std::string						m_defaultMaterialPath = "";
	bool							m_invert = false;
	bool							m_tangents = false;
	float							m_scale = 0.f;
};