#pragma once
#include "Engine/Math/Vec2.hpp"

//Purely for debugging
#include <stdio.h>

struct Camera
{
public:
	//Variables
	Vec2 m_orthoBottomLeft;
	Vec2 m_orthoTopRight;

private:
	//Variables

public:
	//Methods
	void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void Translate2D(const Vec2& translation2D);

private:
	//Methods
};