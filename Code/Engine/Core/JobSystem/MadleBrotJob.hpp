#pragma once
#include "Engine/Core/JobSystem/Job.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Texture.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class MandleBrotJob : public Job 
{
public:
	MandleBrotJob(Image* image, uint rowNum, uint maxIterations);
	~MandleBrotJob();
	void			Execute();

private:
	Image*		m_image = nullptr;
	uint		m_rowNum = 0U;
	uint		m_maxIterations = 0U;
};

//------------------------------------------------------------------------------------------------------------------------------
class UpdateTextureRowJob : public Job 
{
public:
	UpdateTextureRowJob(Image* image, Texture2D* texture2D, uint rowNum);
	~UpdateTextureRowJob();
	void			Execute();

private:
	uint			m_rowNum = 0U;
	Image*			m_image = nullptr;
	Texture2D*		m_textureToUpdate = nullptr;
};