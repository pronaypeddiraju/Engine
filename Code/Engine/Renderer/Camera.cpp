//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight, float minZ, float maxZ)
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
	
	m_minZ = minZ;
	m_maxZ = maxZ;

	printf("Executed SetOrthoView");
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_orthoBottomLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_orthoTopRight;
}

void Camera::Translate2D( const Vec2& translation2D )
{
	m_orthoBottomLeft += translation2D;
	m_orthoTopRight += translation2D;
}

void Camera::SetColorTarget( ColorTargetView *colorTargetView )
{
	m_colorTargetView = colorTargetView;
}

void Camera::UpdateUniformBuffer( RenderContext* renderContext )
{
	if (m_cameraUBO == nullptr) 
	{
		m_cameraUBO = new UniformBuffer( renderContext ); 
	}

	// TODO: This method is called during BeginCamera - so we know our outputs
	// are final - so if you have enough data to figure out an aspect correct
	// projection, you can do it here;  For now, we'll keep with the SD1
	// version that just sets a fixed ortho; 
	CameraBufferT cpuData;
	cpuData.orthoMin = m_orthoBottomLeft; 
	cpuData.orthoMax = m_orthoTopRight; 

	// copy the cpu to the gpu (will create or update the buffer)
	m_cameraUBO->CopyCPUToGPU( &cpuData, sizeof(cpuData) ); 
}

