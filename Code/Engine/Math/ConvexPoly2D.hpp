#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
class ConvexPoly2D
{
public:
	ConvexPoly2D();
	explicit ConvexPoly2D(const std::vector<Vec2>& constructPoints);
	~ConvexPoly2D();

	const std::vector<Vec2>&	GetConvexPoly2DPoints() const;

	void						SetBitFieldsForBitBucketBroadPhase(const IntVec2& bitFields);

private:
	std::vector<Vec2>	m_convexPolyPoints;

	//For broad-phase checks using bitBuckets
	IntVec2				m_bitFieldsXY;
};