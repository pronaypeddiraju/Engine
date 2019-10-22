#include "Engine/Core/JobSystem/ScreenShotJob.hpp"
#include <d3d11.h>
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/JobSystem/WriteImageToFileJob.hpp"
#include "Engine/Core/JobSystem/JobSystem.hpp"
#include "Engine/Commons/Profiler/ProfileLogScope.hpp"

//------------------------------------------------------------------------------------------------------------------------------
ScreenShotJob::ScreenShotJob(Texture2D* backBuffer, const std::string& destinationPath)
{	
	m_backBuffer = backBuffer;
	m_destinationPath = destinationPath;
	m_outTexture = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
ScreenShotJob::~ScreenShotJob()
{
	delete m_backBuffer;
	m_backBuffer = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void ScreenShotJob::Execute()
{
	PROFILE_LOG_SCOPE_FUNCTION();

	m_outTexture = new Texture2D(g_renderContext);
	CopyTextureToCPUImage();

	std::string path = SCREEN_SHOT_PATH;
	path += "/Screenshot_" + GetDateTime() + ".png";

	WriteImageToFileJob* writeJob = new WriteImageToFileJob(m_image, path);

	JobSystem* jobSystem = JobSystem::GetInstance();
	jobSystem->AddJobForCategory(writeJob, JOB_GENERIC);
}

//------------------------------------------------------------------------------------------------------------------------------
void ScreenShotJob::CopyTextureToCPUImage()
{
	PROFILE_LOG_SCOPE_FUNCTION();

	ID3D11DeviceContext* d3dContext = g_renderContext->GetDXContext();
	{
		PROFILE_LOG_SCOPE_FUNCTION();
		m_image = new Image(m_backBuffer->m_dimensions.x, m_backBuffer->m_dimensions.y);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = d3dContext->Map(m_backBuffer->m_handle, 0, D3D11_MAP_READ, 0U, &resource);
	if (SUCCEEDED(hr))
	{
		// we're mapped!  Copy over
		memcpy(m_image->GetWritableImageBuffer(), resource.pData, m_image->GetImageSizeAsSizeT());

		// unlock the resource (we're done writing)
		d3dContext->Unmap(m_backBuffer->m_handle, 0);
	}
	else
	{
		ERROR_RECOVERABLE("Couldn't write image from d3d Resource");
	}
	DX_SAFE_RELEASE(m_backBuffer->m_handle);
}

