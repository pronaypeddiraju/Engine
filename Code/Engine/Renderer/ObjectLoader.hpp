//------------------------------------------------------------------------------------------------------------------------------
#pragma once
// Engine Systems
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/BufferWriteUtils.hpp"
#include "Engine/Core/BufferReadUtils.hpp"
#include "Engine/Commons/EngineCommon.hpp"
// Others
#include <string>
#include <vector>

class CPUMesh;
class GPUMesh;
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
struct ObjIndex
{
	int vertexIndex = 0;
	int uvIndex = 0;
	int normalIndex = 0;
};

//------------------------------------------------------------------------------------------------------------------------------
class ObjectLoader
{
public:
	ObjectLoader();
	~ObjectLoader();
	static ObjectLoader*	MakeLoaderAndLoadMeshFromFile(RenderContext* renderContext, const std::string& filePath, bool isDataDriven);

	void					LoadMeshFromFile(RenderContext* renderContext, const std::string& fileName, bool isDataDriven);
	
	void					LoadFromPMSH(const std::string& fileName, Buffer& readBuffer);
	void					LoadFromXML(const std::string& fileName);
	void					CreateFromString(const char* data);
	void					AddIndexForMesh(const std::string& indices);
	void					CreateCPUMesh();
	void					CreateGPUMesh();

	void					MakeCookedVersion();
public:
	std::vector<Vec3>				m_positions;
	std::vector<Vec2>				m_uvs;
	std::vector<Vec3>				m_normals;
	std::vector<ObjIndex>			m_indices;
	
	RenderContext*					m_renderContext = nullptr;
	CPUMesh*						m_cpuMesh = nullptr;
	GPUMesh*						m_mesh = nullptr;

	std::string						m_source = "";
	std::string						m_fullFileName = "";
	std::string						m_transform = "";
	std::string						m_defaultMaterialPath = "";
	bool							m_invert = false;
	bool							m_tangents = false;
	bool							m_isCooked = false;
	float							m_scale = 0.f;

	bool							m_cookingRun = RUN_COOKING;
};