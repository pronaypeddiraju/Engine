//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Ray3D.hpp"

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

//------------------------------------------------------------------------------------------------------------------------------
Ray3D Camera::ScreenPointToWorldRay(IntVec2 ScreenClickPosition, IntVec2 screenBounds)
{
	// step 1 convert from screen to NDC
	float NDCx = RangeMapFloat((float)ScreenClickPosition.x, 0.f, (float)screenBounds.x, -1.f, 1.f);
	float NDCy = RangeMapFloat((float)ScreenClickPosition.y, 0.f, (float)screenBounds.y, 1.f, -1.f);

	// step 2 covert from NDC to clip
	Vec4 start = Vec4(NDCx, NDCy, 0.f, 1.f);
	Vec4 end = Vec4(NDCx, NDCy, 1.f, 1.f);

	// step 3 convert from clip to view 
	Matrix44 inverseProjection = m_projection;
	inverseProjection.InverseMatrix();
	
	start *= inverseProjection;
	end *= inverseProjection;
	
	// step 4 convert from view to world
	Matrix44 inverseView = m_view;
	inverseView.InverseMatrix();
	 
	start *= inverseView;
	end *= inverseView;

	Vec3 start3 = Vec3(start.x, start.y, start.z) / start.w;
	Vec3 end3 = Vec3(end.x, end.y, end.z) / end.w;

	Vec3 direction = (end3 - start3).GetNormalized();
	Ray3D ray(start3, direction);
	return ray;
}
