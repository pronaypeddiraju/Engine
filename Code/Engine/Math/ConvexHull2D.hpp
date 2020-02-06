//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/ConvexPoly2D.hpp"
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
class ConvexHull2D
{
public:
	ConvexHull2D();
	explicit ConvexHull2D(const std::vector<Plane2D>& outerSlices);
	~ConvexHull2D();

	void							MakeConvexHullFromConvexPolyon(const ConvexPoly2D& polygon);
	int								GetNumPlanes() const;
	const std::vector<Plane2D>&		GetPlanes() const;

	void							PushPlane(const Plane2D& plane);

	void							SetBitFieldsForBitBucketBroadPhase(const IntVec2& bitFields);
private:
	std::vector<Plane2D> m_outerSlices;

	//For broad-phase checks using bitBuckets
	IntVec2				m_bitFieldsXY;
public:
};