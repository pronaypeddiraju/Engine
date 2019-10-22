#pragma once
#include "Engine/Core/JobSystem/Job.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Image.hpp"
#include <string.h>

class ScreenShotJob : public Job
{
public:
	ScreenShotJob(Texture2D* backBuffer, const std::string& destinationPath);
	~ScreenShotJob();
	void			Execute();

private:
	Image*			m_image = nullptr;
	Texture2D*		m_backBuffer = nullptr;
	Texture2D*		m_outTexture = nullptr;
	std::string		m_destinationPath = "Data/Images/ScreenShots/";
	
	void			CopyTextureToCPUImage();
};