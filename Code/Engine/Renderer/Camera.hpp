#pragma once
#include "Engine/Math/Vec2.hpp"

//Purely for debugging
#include <stdio.h>

class ColorTargetView;

struct Camera
{

public:

	void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void Translate2D(const Vec2& translation2D);

	//Funtion to set the color target view that we are rendering to
	void SetColorTarget(ColorTargetView *colorTargetView);

public:
	//Variables
	Vec2 m_orthoBottomLeft;
	Vec2 m_orthoTopRight;

	ColorTargetView		*m_colorTargetView;			//What the camera will render to

};