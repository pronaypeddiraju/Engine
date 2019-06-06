//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Frustum.hpp"
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

//------------------------------------------------------------------------------------------------------------------------------
Frustum Camera::GetWorldFrustum() const
{
	AABB3 ndc = AABB3(Vec3(-1, -1, 0), Vec3(1, 1, 1));
	
	Vec4 min4 = Vec4(-1.f, -1.f, 0.f, 1.f);
	Vec4 max4 = Vec4(1.f, 1.f, 1.f, 1.f);

	Matrix44 inverseProjection = GetProjectionMatrix();
	inverseProjection.InverseMatrix();
	Matrix44 inverseView = GetViewMatrix();
	inverseView.InverseMatrix();

	//	Order
	//    5-----6
	//   /|    /|
	//  1-----2 |
	//  | 4---|-7
	//  |/    |/
	//  0-----3

	Vec3 corners[8];
	Vec4 corners4D[8];

	ndc.GetCornersForAABB3(&corners[0]);
	for (int cornerIndex = 0; cornerIndex < 8; cornerIndex++)
	{
		corners4D[cornerIndex] = Vec4(corners[cornerIndex].x, corners[cornerIndex].y, corners[cornerIndex].z, 1.f);
		corners4D[cornerIndex] = inverseProjection.TransformHomogeneousPoint3D(corners4D[cornerIndex]);
		corners4D[cornerIndex] = inverseView.TransformHomogeneousPoint3D(corners4D[cornerIndex]);

		//Div by w
		float invertW = 1 / corners4D[cornerIndex].w;
		corners4D[cornerIndex].x *= invertW;
		corners4D[cornerIndex].y *= invertW;
		corners4D[cornerIndex].z *= invertW;
		corners4D[cornerIndex].w = 1.f;

		//Store in the Vec3 array
		corners[cornerIndex] = Vec3(corners4D[cornerIndex].x, corners4D[cornerIndex].y, corners4D[cornerIndex].z);
	}

	Frustum viewFrustum;

	// LH for left-handed basis - hence left handed winding order
	viewFrustum.m_planes[FRUSTUM_LEFT] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[4], corners[5]);
	viewFrustum.m_planes[FRUSTUM_RIGHT] = Plane3D::MakeFromTriangleLHRule(corners[3], corners[6], corners[7]);
	viewFrustum.m_planes[FRUSTUM_TOP] = Plane3D::MakeFromTriangleLHRule(corners[1], corners[5], corners[6]);
	viewFrustum.m_planes[FRUSTUM_BOTTOM] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[7], corners[4]);
	viewFrustum.m_planes[FRUSTUM_FRONT] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[1], corners[2]);
	viewFrustum.m_planes[FRUSTUM_BACK] = Plane3D::MakeFromTriangleLHRule(corners[4], corners[7], corners[6]);	

	return viewFrustum;
}

//------------------------------------------------------------------------------------------------------------------------------
Frustum Camera::GetWorldFrustumFromClientRegion(const AABB2& clientRegion)
{
	Vec3 min = ClientToNDC(clientRegion.m_minBounds);
	Vec3 max = ClientToNDC(clientRegion.m_maxBounds);
	max.z = 1.f;

	Vec3 newMin = Vec3::GetComponentMin(min, max);
	Vec3 newMax = Vec3::GetComponentMax(min, max);

	// step 2 covert from NDC to clip
	Vec4 min4 = Vec4(newMin.x, newMin.y, 0.f, 1.f);
	Vec4 max4 = Vec4(newMax.x, newMax.y, 1.f, 1.f);

	AABB3 subNDC = AABB3(newMin, newMax);

	Matrix44 worldtoNDC = GetViewMatrix();
	worldtoNDC = worldtoNDC.AppendMatrix(GetProjectionMatrix());

	Matrix44 NDCtoWorld = worldtoNDC;
	NDCtoWorld.InverseMatrix();

	Matrix44 inverseProjection = GetProjectionMatrix();
	inverseProjection.InverseMatrix();
	Matrix44 inverseView = GetViewMatrix();
	inverseView.InverseMatrix();

	//	Order
	//    5-----6
	//   /|    /|
	//  1-----2 |
	//  | 4---|-7
	//  |/    |/
	//  0-----3
	Vec3 corners[8];
	Vec4 corners4D[8];

	subNDC.GetCornersForAABB3(&corners[0]);
	for (int cornerIndex = 0; cornerIndex < 8; cornerIndex++)
	{
		corners4D[cornerIndex] = Vec4(corners[cornerIndex].x, corners[cornerIndex].y, corners[cornerIndex].z, 1.f);
		corners4D[cornerIndex] = inverseProjection.TransformHomogeneousPoint3D(corners4D[cornerIndex]);
		corners4D[cornerIndex] = inverseView.TransformHomogeneousPoint3D(corners4D[cornerIndex]);
		
		//Div by w
		float invertW = 1 / corners4D[cornerIndex].w;
		corners4D[cornerIndex].x *= invertW;
		corners4D[cornerIndex].y *= invertW;
		corners4D[cornerIndex].z *= invertW;
		corners4D[cornerIndex].w = 1.f;

		//Store in the Vec3 array
		corners[cornerIndex] = Vec3(corners4D[cornerIndex].x, corners4D[cornerIndex].y, corners4D[cornerIndex].z);
	}

	Frustum viewFrustum;

	// LH for left-handed basis - hence left handed winding order
	viewFrustum.m_planes[FRUSTUM_LEFT] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[4], corners[5]);
	viewFrustum.m_planes[FRUSTUM_RIGHT] = Plane3D::MakeFromTriangleLHRule(corners[3], corners[6], corners[7]);
	viewFrustum.m_planes[FRUSTUM_TOP] = Plane3D::MakeFromTriangleLHRule(corners[1], corners[5], corners[6]);
	viewFrustum.m_planes[FRUSTUM_BOTTOM] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[7], corners[4]);
	viewFrustum.m_planes[FRUSTUM_FRONT] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[1], corners[2]);
	viewFrustum.m_planes[FRUSTUM_BACK] = Plane3D::MakeFromTriangleLHRule(corners[4], corners[7], corners[6]);

	return viewFrustum;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3 Camera::ClientToNDC(const Vec2& clientPos)
{
	Vec3 ndcPos = Vec3::ZERO;

	IntVec2 screenBounds = g_windowContext->GetTrueClientBounds();
	float NDCx = RangeMapFloat((float)clientPos.x, 0.f, (float)screenBounds.x, -1.f, 1.f);
	float NDCy = RangeMapFloat((float)clientPos.y, 0.f, (float)screenBounds.y, 1.f, -1.f);

	ndcPos.x = NDCx;
	ndcPos.y = NDCy;

	return ndcPos;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Camera::GetCameraRight() const
{
	return m_cameraModel.GetIVector();
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Camera::GetCameraUp() const
{
	return m_cameraModel.GetJVector();
}
