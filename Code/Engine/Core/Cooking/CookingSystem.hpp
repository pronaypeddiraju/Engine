#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class CookingSystem
{
public:
	CookingSystem();
	~CookingSystem();

	void	CookMeshesUnderDirectory(const std::string& meshDir = "Data/Models/");

	void	CookGPUMesh(const GPUMesh& mesh);

private:
	bool	m_isCooking = false;


};
