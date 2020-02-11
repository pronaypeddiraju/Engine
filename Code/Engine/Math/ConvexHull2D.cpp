//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/ConvexHull2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
ConvexHull2D::ConvexHull2D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
ConvexHull2D::ConvexHull2D(const std::vector<Plane2D>& outerSlices)
{
	m_outerSlices = outerSlices;
}

//------------------------------------------------------------------------------------------------------------------------------
ConvexHull2D::~ConvexHull2D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void ConvexHull2D::MakeConvexHullFromConvexPolyon(const ConvexPoly2D& polygon)
{
	//Traverse all the points in the convexPolygon and make a convex hull from there
	std::vector<Vec2> points = polygon.GetConvexPoly2DPoints();

	for (int pointIndex = points.size() - 1; pointIndex > 0; pointIndex--)
	{
		//Get the vector between the 2 points
		Vec2 displacementVector = points[pointIndex] - points[pointIndex - 1];
		//Plane normal is the vector rotated by -90 degrees
		Vec2 planeNormal = displacementVector.GetRotatedMinus90Degrees();
		planeNormal.Normalize();

		//Make the plane using the normal and some point on the plane
		Plane2D plane(planeNormal, points[pointIndex]);
		m_outerSlices.push_back(plane);
	}

	//We still need to add the last plane containing the 1st and last point of the convex polygon
	Vec2 displacementVector = points[0] - points[points.size() - 1];
	Vec2 planeNormal = displacementVector.GetRotatedMinus90Degrees();
	planeNormal.Normalize();
	Plane2D plane(planeNormal, points[0]);
	m_outerSlices.push_back(plane);
}

//------------------------------------------------------------------------------------------------------------------------------
int ConvexHull2D::GetNumPlanes() const
{
	return (int)m_outerSlices.size();
}

//------------------------------------------------------------------------------------------------------------------------------
const std::vector<Plane2D>& ConvexHull2D::GetPlanes() const
{
	return m_outerSlices;
}

//------------------------------------------------------------------------------------------------------------------------------
void ConvexHull2D::PushPlane(const Plane2D& plane)
{
	m_outerSlices.push_back(plane);
}

//------------------------------------------------------------------------------------------------------------------------------
void ConvexHull2D::SetBitFieldsForBitBucketBroadPhase(const IntVec2& bitFields)
{
	m_bitFieldsXY = bitFields;
}

//------------------------------------------------------------------------------------------------------------------------------
const IntVec2& ConvexHull2D::GetBitFields() const
{
	return m_bitFieldsXY;
}

