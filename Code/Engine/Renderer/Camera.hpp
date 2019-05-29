//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class ColorTargetView;
class DepthStencilTargetView;
class RenderContext;
class UniformBuffer;
struct AABB2;
struct Frustum;
struct IntVec2;
struct Ray3D;

//------------------------------------------------------------------------------------------------------------------------------
struct Camera
{
public:
	~Camera();

	void					Translate2D(const Vec2& translation2D);

	// Projections
	void					SetPerspectiveProjection(float FieldOfView, float nearZ, float farZ, float aspectRatio = 16.f/9.f);
	void					SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight, float minZ = -1.f, float maxZ = 1.f);
	Vec2					GetOrthoBottomLeft() const;
	Vec2					GetOrthoTopRight() const;
	inline float			GetMaxViewDistance() const { return m_maxZ; }

	//Targets
	//Function to set the color target view that we are rendering to
	void					SetColorTarget(ColorTargetView *colorTargetView);
	void					SetDepthStencilTarget( DepthStencilTargetView *depthStencilView);

	//Utility
	inline const Vec3&		GetEuler() {return m_Euler;}
	inline void				SetEuler(const Vec3& euler) {m_Euler = euler;}

	//Transforms and Matrices
	void					SetModelMatrix(Matrix44 camModel);
	const Matrix44&			GetModelMatrix() const;
	const Matrix44&			GetViewMatrix() const;
	const Matrix44&			GetProjectionMatrix() const;

	//Buffer functions
	void					UpdateUniformBuffer(RenderContext* renderContext);

	//Raycast utilities
	Ray3D					ScreenPointToWorldRay(IntVec2 ScreenClickPosition, IntVec2 screenBounds);

	//Frustums
	Frustum					GetWorldFrustum() const;
	Frustum					GetWorldFrustumFromClientRegion(const AABB2& clientRegion);

	Vec3					ClientToNDC(const Vec2& clientPos);
public:
	//Variables
	Vec2						m_orthoBottomLeft;
	Vec2						m_orthoTopRight;

	float						m_minZ = 0.f;
	float						m_maxZ = 0.f;

	//Render Targets
	ColorTargetView				*m_colorTargetView;			//What the camera will render to
	DepthStencilTargetView		*m_depthStencilView;

	// UniformBuffer that will store our camera information
	UniformBuffer				*m_cameraUBO    = nullptr; // UBO -> Uniform Buffer Object

	//Projection Type (Optional)
	//ProjectionInfoT			m_projectionInfo;

	//Cached Matrices
	Matrix44					m_projection;
	Matrix44					m_cameraModel;
	Matrix44					m_view;			//This is the inverse of m_camera

	Vec3						m_Euler = Vec3::ZERO;
};
