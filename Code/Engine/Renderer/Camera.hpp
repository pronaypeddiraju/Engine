#pragma once
#include "Engine/Math/Vec2.hpp"

//Purely for debugging
#include <stdio.h>

class ColorTargetView;
class RenderContext;
class UniformBuffer;

struct Camera
{
public:
	~Camera();

	void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight, float minZ = -1.f, float maxZ = 1.f);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void Translate2D(const Vec2& translation2D);

	//Funtion to set the color target view that we are rendering to
	void SetColorTarget(ColorTargetView *colorTargetView);

	void UpdateUniformBuffer(RenderContext* renderContext);

public:
	//Variables
	Vec2				m_orthoBottomLeft;
	Vec2				m_orthoTopRight;

	float				m_minZ = 0.f;
	float				m_maxZ = 0.f;

	ColorTargetView		*m_colorTargetView;			//What the camera will render to

	// UniformBuffer that will store our camera information
	UniformBuffer *m_cameraUBO    = nullptr; // UBO -> Uniform Buffer Object

};