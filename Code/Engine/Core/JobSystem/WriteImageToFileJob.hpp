#pragma once
#include "Engine/Core/JobSystem/Job.hpp"
#include "Engine/Core/Image.hpp"
#include "ThirdParty/stb/stb_image_write.h"

//------------------------------------------------------------------------------------------------------------------------------
class WriteImageToFileJob : public Job
{
public:
	WriteImageToFileJob(Image* imageRef, const std::string& destinationPath);
	~WriteImageToFileJob();
	void			Execute();

private:
	Image*			m_image = nullptr;
	std::string		m_destinationPath = "Data/Images/ScreenShots/";
};

//------------------------------------------------------------------------------------------------------------------------------
WriteImageToFileJob::WriteImageToFileJob(Image* imageRef, const std::string& destinationPath)
{
	m_image = imageRef;
	m_destinationPath = destinationPath;
}

//------------------------------------------------------------------------------------------------------------------------------
WriteImageToFileJob::~WriteImageToFileJob()
{
	delete m_image;
	m_image = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void WriteImageToFileJob::Execute()
{
	IntVec2 imageDims = m_image->GetImageDimensions();

	stbi_write_png(m_destinationPath.c_str(), imageDims.x, imageDims.y, m_image->GetBytesPerPixel(), m_image->GetImageBuffer(), imageDims.x * m_image->GetBytesPerPixel());
}
