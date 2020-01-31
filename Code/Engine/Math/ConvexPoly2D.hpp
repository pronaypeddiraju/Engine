#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
class ConvexPoly2D
{
public:
	ConvexPoly2D();
	explicit ConvexPoly2D(const std::vector<Vec2>& constructPoints);
	~ConvexPoly2D();

	const std::vector<Vec2>&	GetConvexPoly2DPoints();

private:
	std::vector<Vec2>	m_convexPolyPoints;
};