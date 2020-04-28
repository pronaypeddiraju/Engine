#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class CookingSystem
{
public:
	CookingSystem();
	~CookingSystem();

	void	CookMeshesUnderDirectory(const std::string& meshDir = "Data/Models/");

private:
	bool	m_isCooking = false;


};
