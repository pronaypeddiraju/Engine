#include "Engine/Math/ConvexPoly2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
ConvexPoly2D::ConvexPoly2D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
ConvexPoly2D::ConvexPoly2D(const std::vector<Vec2>& constructPoints)
{
	m_convexPolyPoints = constructPoints;
}

//------------------------------------------------------------------------------------------------------------------------------
ConvexPoly2D::~ConvexPoly2D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
const std::vector<Vec2>& ConvexPoly2D::GetConvexPoly2DPoints()
{
	return m_convexPolyPoints;
}

