#include "Engine/Core/JobSystem/MadleBrotJob.hpp"
#include <d3d11.h>
#include "Engine/Renderer/RenderContext.hpp"

//------------------------------------------------------------------------------------------------------------------------------
MandleBrotJob::MandleBrotJob(Image* image, uint rowNum, uint maxIterations)
{
	m_rowNum = rowNum;
	m_image = image;
	m_maxIterations = maxIterations;
}

//------------------------------------------------------------------------------------------------------------------------------
MandleBrotJob::~MandleBrotJob()
{

}

//------------------------------------------------------------------------------------------------------------------------------
static uint CheckMandlebrotSet(float x0, float y0, uint const MAX_ITERATIONS)
{

	float x = 0.0f;
	float y = 0.0f;

	uint iteration = 0;
	while ((((x * x) + (y * y)) < 4.0f) && (iteration < MAX_ITERATIONS)) 
	{
		float xtemp = x * x - y * y + x0;
		y = 2.0f * x * y + y0;
		x = xtemp;
		iteration++;
	}

	return iteration;
}

//------------------------------------------------------------------------------------------------------------------------------
void MandleBrotJob::Execute()
{
	//Execute MandleBrot generation for a specific row
	//void* rowPtr = m_image->GetRawPointerToRow(m_rowNum);

	IntVec2 imageDim = m_image->GetImageDimensions();

	// column stuff
	// scale image height to -1 to 1 range for mandelbrot
	float y0 = ((float)m_rowNum / (float)imageDim.y) * 2.0f - 1.0f;

	for (uint x = 0; x < (uint)imageDim.x; ++x)
	{
		//scale image width 
		float x0 = ((float)x / (float)imageDim.x) * 3.5f - 2.5f;

		uint iterations = CheckMandlebrotSet(x0, y0, m_maxIterations);

		// feel free to tone-map by normlizing iteration count
		// for a more colourful image... but black and white
		// is fine for this assignment; 
		if (iterations == m_maxIterations) 
		{
			m_image->SetTexelColor(x, m_rowNum, Rgba::BLACK);
		}
		else 
		{
			m_image->SetTexelColor(x, m_rowNum, Rgba::WHITE);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
UpdateTextureRowJob::UpdateTextureRowJob(Image* image, Texture2D* texture2D, uint rowNum)
{
	m_rowNum = rowNum;
	m_image = image;
	m_textureToUpdate = texture2D;
}

//------------------------------------------------------------------------------------------------------------------------------
UpdateTextureRowJob::~UpdateTextureRowJob()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void UpdateTextureRowJob::Execute()
{
	// map the image
	ID3D11DeviceContext* d3dContext = g_renderContext->GetDXContext();

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HRESULT hr = d3dContext->Map(m_textureToUpdate->m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource);
	if (SUCCEEDED(hr))
	{
		// we're mapped!  Copy over
		void* buffer = m_image->GetRawPointerToRow(m_rowNum);
		void* texBuffer = (unsigned char*)resource.pData + m_rowNum * resource.RowPitch;

		memcpy(texBuffer, buffer, m_image->GetImageDimensions().x * m_image->GetBytesPerPixel());

		// unlock the resource (we're done writing)
		d3dContext->Unmap(m_textureToUpdate->m_handle, 0);
	}
	else
	{
		ERROR_RECOVERABLE("Couldn't write image from d3d Resource");
	}
}
