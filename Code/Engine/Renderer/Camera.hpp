//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Matrix44.hpp"

class ColorTargetView;
class DepthStencilTargetView;
class RenderContext;
class UniformBuffer;

//------------------------------------------------------------------------------------------------------------------------------
struct Camera
{
public:
	~Camera();

	void			Translate2D(const Vec2& translation2D);

	// Projections
	void			SetPerspectiveProjection(float FieldOfView, float nearZ, float farZ, float aspectRatio = 16.f/9.f);
	void			SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight, float minZ = -1.f, float maxZ = 1.f);
	Vec2			GetOrthoBottomLeft() const;
	Vec2			GetOrthoTopRight() const;

	//Targets
	//Function to set the color target view that we are rendering to
	void			SetColorTarget(ColorTargetView *colorTargetView);
	void			SetDepthStencilTarget( DepthStencilTargetView *depthStencilView);

	//Transforms and Matrices
	void			SetModelMatrix(Matrix44 camModel);
	const Matrix44&	GetModelMatrix() const;
	const Matrix44&	GetViewMatrix() const;
	const Matrix44&	GetProjectionMatrix() const;

	//Buffer functions
	void			UpdateUniformBuffer(RenderContext* renderContext);

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
	Matrix44					m_camera;
	Matrix44					m_view;			//This is the inverse of m_camera
};
