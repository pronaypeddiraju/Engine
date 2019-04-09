//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
	if(m_cameraUBO != nullptr)
	{
		delete m_cameraUBO;
		m_cameraUBO = nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight, float minZ, float maxZ)
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
	
	m_minZ = minZ;
	m_maxZ = maxZ;

	//Set the camera projection to be an ortho matrix
	m_projection = Matrix44::MakeOrthoMatrix(bottomLeft, topRight, minZ, maxZ);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_orthoBottomLeft;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthoTopRight() const
{
	return m_orthoTopRight;
}

//------------------------------------------------------------------------------------------------------------------------------
void Camera::Translate2D( const Vec2& translation2D )
{
	m_orthoBottomLeft += translation2D;
	m_orthoTopRight += translation2D;

	SetOrthoView(m_orthoBottomLeft, m_orthoTopRight);
}

//------------------------------------------------------------------------------------------------------------------------------
void Camera::SetPerspectiveProjection( float FieldOfView, float nearZ, float farZ, float aspectRatio)
{
	m_projection = Matrix44::MakePerspectiveMatrix(FieldOfView, nearZ, farZ, aspectRatio);
}

//------------------------------------------------------------------------------------------------------------------------------
void Camera::SetColorTarget( ColorTargetView *colorTargetView )
{
	m_colorTargetView = colorTargetView;
}

//------------------------------------------------------------------------------------------------------------------------------
void Camera::SetDepthStencilTarget( DepthStencilTargetView *depthStencilView )
{
	m_depthStencilView = depthStencilView;
}

//------------------------------------------------------------------------------------------------------------------------------
void Camera::SetModelMatrix( Matrix44 camModel )
{
	m_cameraModel = camModel;

	m_view = Matrix44::InvertOrthoNormal(m_cameraModel);
}

//------------------------------------------------------------------------------------------------------------------------------
const Matrix44& Camera::GetModelMatrix() const
{
	return m_cameraModel;
}

//------------------------------------------------------------------------------------------------------------------------------
const Matrix44& Camera::GetViewMatrix() const
{
	return m_view;
}

//------------------------------------------------------------------------------------------------------------------------------
const Matrix44& Camera::GetProjectionMatrix() const
{
	return m_projection;
}

//------------------------------------------------------------------------------------------------------------------------------
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
	cpuData.ViewMatrix = m_view; 
	cpuData.ProjectionMatrix = m_projection; 
	cpuData.CameraPosition = m_cameraModel.GetTVector();

	// copy the cpu to the gpu (will create or update the buffer)
	m_cameraUBO->CopyCPUToGPU( &cpuData, sizeof(cpuData) ); 
}