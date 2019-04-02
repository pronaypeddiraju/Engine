//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Manifold.hpp"
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/Segment2D.hpp"
#include "Engine/Renderer/DebugRender.hpp"

CollisionCheck2DCallback COLLISION_LOOKUP_TABLE[COLLIDER2D_COUNT][COLLIDER2D_COUNT] = {
	/*******| aabb2 | disc  | capsl | obb2 | line  | point  */
	/*aabb2*/ { CheckAABB2ByAABB2, CheckAABB2ByDisc, nullptr,				nullptr,			nullptr, nullptr },
	/*disc */ { CheckDiscByAABB2,  CheckDiscByDisc,  nullptr,				nullptr,			nullptr, nullptr },
	/*capsl*/ { nullptr,           nullptr,          CheckCapsuleByCapsule, CheckCapsuleByOBB2,	nullptr, nullptr },
	/*obb2*/  { nullptr,           nullptr,          CheckOBB2ByCapsule,	CheckOBB2ByOBB2,	nullptr, nullptr },
	/*line*/  { nullptr,           nullptr,          nullptr,				nullptr,			nullptr, nullptr },
	/*point*/ { nullptr,           nullptr,          nullptr,				nullptr,			nullptr, nullptr },
}; 

//------------------------------------------------------------------------
// Doing this by lookup - as it is a good intro to callbacks
// but could also do this by double dispatch:  a->collides_with( b )
bool GetCollisionInfo( Collision2D* out, Collider2D* a, Collider2D* b )
{
	uint aType = a->GetType(); 
	uint bType = b->GetType(); 

	if(aType >= COLLIDER2D_COUNT && bType >= COLLIDER2D_COUNT)
	{
		ERROR_AND_DIE("The Collider type was not part of the COLLISION_LOOKUP_TABLE");
	}

	CollisionCheck2DCallback callBack = COLLISION_LOOKUP_TABLE[aType][bType]; 

	if (callBack == nullptr) 
	{
		return false; // no known collision; 
	}
	else 
	{
		return callBack( out, a, b ); 
	}
}

bool GetManifold( Manifold2D *out, AABB2Collider const &boxA, AABB2Collider const &boxB )
{
	//Get the intersecting box
	Vec2 min = boxA.GetWorldShape().m_maxBounds.Min(boxB.GetWorldShape().m_maxBounds);
	Vec2 max = boxA.GetWorldShape().m_minBounds.Max(boxB.GetWorldShape().m_minBounds);

	//AABB2 collisionBox = AABB2(max, min);

	if(max < min)
	{
		GenerateManifoldBoxToBox(out, min, max);

		AABB2 boxAShape = boxA.GetWorldShape();
		AABB2 boxBShape = boxB.GetWorldShape();

		if(out->m_normal.y == 0.f)
		{	
			if(((boxAShape.m_maxBounds + boxAShape.m_minBounds)/2).x < ((boxBShape.m_maxBounds + boxBShape.m_minBounds)/2).x)
			{
				//pushing out on x
				out->m_normal *= -1;
			}
		}
		else if(out->m_normal.x == 0.f)
		{
			if(((boxAShape.m_maxBounds + boxAShape.m_minBounds)/2).y < ((boxBShape.m_maxBounds + boxBShape.m_minBounds)/2).y)
			{
				//pushing out on y
				out->m_normal *= -1;
			}
		}

		return true;
	}
	else 
	{
		return false;
	}
}

bool GetManifold( Manifold2D *out, AABB2Collider const &box, Disc2DCollider const &disc )
{
	Vec2 discCentre = disc.GetWorldShape().GetCentre();
	AABB2 boxShape = box.GetWorldShape();
	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxShape );
	Vec2 boxCenter = boxShape.GetBoxCenter() + boxShape.m_minBounds;

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = disc.GetWorldShape().GetRadius();
	//float distanceBwCenters = GetDistanceSquared2D(discCentre, boxCenter);

	float distance = 0;

	//Check is box inside disc
	if(closestPoint == discCentre)
	{
		//box is inside disc
		distance = GetDistance2D(discCentre, boxCenter);
		Vec2 normal = boxCenter - discCentre;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = distance;
		return true;
	}

	if(distanceSquared < radius * radius)
	{
		//out here
		distance = GetDistance2D(discCentre, closestPoint);
		Vec2 normal = closestPoint - discCentre;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = radius - distance;
		return true;
	}
	else
	{
		return false;
	}
}

bool GetManifold( Manifold2D *out, Disc2DCollider const &disc, AABB2Collider const &box)
{
	Vec2 discCentre = disc.GetWorldShape().GetCentre();
	AABB2 boxShape = box.GetWorldShape();

	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxShape );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = disc.GetWorldShape().GetRadius();

	if(closestPoint == discCentre)
	{
		return IsDiscInBox(out, discCentre, boxShape, radius);
	}

	if(distanceSquared < radius * radius)
	{
		//out here
		float distance = GetDistance2D(discCentre, closestPoint);

		Vec2 normal = discCentre - closestPoint;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = radius - distance;
		return true;
	}
	else
	{
		return false;
	}
}

bool GetManifold( Manifold2D *out, OBB2 const &boxA, OBB2 const &boxB)
{
	Plane2D planesOfThis[4];    // p0
	Plane2D planesOfOther[4];   // p1

	Vec2 cornersOfThis[4];     // c0
	Vec2 cornersOfOther[4];    // c1

	boxA.GetPlanes( planesOfThis ); 
	boxA.GetCorners( cornersOfThis ); 

	boxB.GetPlanes( planesOfOther ); 
	boxB.GetCorners( cornersOfOther ); 

	int inFrontOfThis = 0;
	int inFrontOfOther = 0;

	//Data for the manifold generation
	float bestDistToA = 100000; 
	Vec2 bestPointToA;

	float bestDistToB = 100000; 
	Vec2 bestPointToB;
	/*
	float smallestDistance = -100000;
	Plane2D planeToPushOutOf;
	*/

	Plane2D bestThisPlane;
	Plane2D bestOtherPlane;

	Vec2 worstCornerToPlane[4];
	float worstDistancesToThis[4];
	Vec2 worstCornerToOtherPlane[4];
	float worstDistancesToOther[4];

	for(int planeIndex = 0; planeIndex < 4;planeIndex++)
	{
		Plane2D const &thisPlane = planesOfThis[planeIndex];
		Plane2D const &otherPlane = planesOfOther[planeIndex];

		inFrontOfThis = 0;
		inFrontOfOther = 0;

		for(int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
		{
			Vec2 const &cornerOfThis = cornersOfThis[cornerIndex]; 
			Vec2 const &cornerOfOther = cornersOfOther[cornerIndex];

			float otherFromThis = thisPlane.GetDistance( cornerOfOther ); 
			float thisFromOther = otherPlane.GetDistance( cornerOfThis ); 


			if(cornerIndex == 0)
			{
				bestDistToA = otherFromThis;
				bestPointToA = cornerOfOther;
				bestThisPlane = thisPlane;

				worstCornerToPlane[planeIndex] = cornerOfOther;
				worstDistancesToThis[planeIndex] = otherFromThis;

				bestDistToB = thisFromOther;
				bestPointToB = cornerOfThis;
				bestOtherPlane = otherPlane;

				worstCornerToOtherPlane[planeIndex] = cornerOfThis;
				worstDistancesToOther[planeIndex] = thisFromOther;
			}

			//Update the feature point
			if(otherFromThis < bestDistToA)
			{
				bestDistToA = otherFromThis;
				bestPointToA = cornerOfOther;
				bestThisPlane = thisPlane;

				worstCornerToPlane[planeIndex] = cornerOfOther;
				worstDistancesToThis[planeIndex] = otherFromThis;
			}

			//Update the feature point for other plane
			if(thisFromOther < bestDistToB)
			{
				bestDistToB = thisFromOther;
				bestPointToB = cornerOfThis;
				bestOtherPlane = otherPlane;

				worstCornerToOtherPlane[planeIndex] = cornerOfThis;
				worstDistancesToOther[planeIndex] = thisFromOther;
			}

			/*
			//Get the largest negative value as distance to push out of
			if(otherFromThis < 0)
			{
			if(otherFromThis > smallestDistance)
			{
			smallestDistance = otherFromThis;
			planeToPushOutOf = thisPlane;
			}
			}
			*/

			inFrontOfThis += (otherFromThis >= 0.0f) ? 1 : 0; 
			inFrontOfOther += (thisFromOther >= 0.0f) ? 1 : 0; 
		}

		//We are not intersecting if there are exactly 4 in front of either plane. Early out bro
		if ((inFrontOfThis == 4) || (inFrontOfOther == 4)) {
			return false;
		}
	}

	float bestCaseThis = 0.f;
	float bestCaseOther = 0.f;
	int bestCaseIndexThis = 0;
	int bestCaseIndexOther = 0;

	for(int worstCaseIndex = 0; worstCaseIndex < 4; worstCaseIndex++)
	{
		if(worstCaseIndex == 0)
		{
			bestCaseThis = worstDistancesToThis[worstCaseIndex];
			bestCaseOther = worstDistancesToOther[worstCaseIndex];
		}

		if(worstDistancesToThis[worstCaseIndex] > bestCaseThis)
		{
			bestCaseThis = worstDistancesToThis[worstCaseIndex];
			bestCaseIndexThis = worstCaseIndex;
		}

		if(worstDistancesToOther[worstCaseIndex] > bestCaseOther)
		{
			bestCaseOther = worstDistancesToOther[worstCaseIndex];
			bestCaseIndexOther = worstCaseIndex;
		}
	}

	//Check which of the 2 are larger (smaller -ve number)
	if(bestCaseOther > bestCaseThis)
	{
		//We have the contact point now
		out->m_penetration = bestCaseOther * -1.f;
		
		out->m_normal = planesOfOther[bestCaseIndexOther].m_normal;
	}
	else
	{
		//We have the contact point now
		out->m_penetration = bestCaseThis * -1.f;

		out->m_normal = planesOfThis[bestCaseIndexThis].m_normal * -1.f;
	}

	return true; 
}

bool GetManifold( Manifold2D *out, BoxCollider2D const &a, BoxCollider2D const &b )
{
	OBB2 boxA = a.GetWorldShape();
	OBB2 boxB = b.GetWorldShape();

	Plane2D planesOfThis[4];    // p0
	Plane2D planesOfOther[4];   // p1

	Segment2D segmentsOfThis[4];
	Segment2D segmentsOfOther[4];

	Vec2 cornersOfThis[4];     // c0
	Vec2 cornersOfOther[4];    // c1

	boxA.GetPlanes( planesOfThis ); 
	boxA.GetCorners( cornersOfThis ); 
	boxA.GetSides (segmentsOfThis );

	boxB.GetPlanes( planesOfOther ); 
	boxB.GetCorners( cornersOfOther ); 
	boxB.GetSides( segmentsOfOther );

	int inFrontOfThis = 0;
	int inFrontOfOther = 0;

	//Data for the manifold generation
	float bestDistToA = 100000; 
	Vec2 bestPointToA;
	
	float bestDistToB = 100000; 
	Vec2 bestPointToB;
	/*
	float smallestDistance = -100000;
	Plane2D planeToPushOutOf;
	*/

	Plane2D bestThisPlane;
	Plane2D bestOtherPlane;

	Segment2D bestSegmentThis;
	Segment2D bestSegmentOther;

	Vec2 worstCornerToPlane[4];
	float worstDistancesToThis[4];
	Vec2 worstCornerToOtherPlane[4];
	float worstDistancesToOther[4];

	for(int planeIndex = 0; planeIndex < 4;planeIndex++)
	{
		Plane2D const &thisPlane = planesOfThis[planeIndex];
		Plane2D const &otherPlane = planesOfOther[planeIndex];

		inFrontOfThis = 0;
		inFrontOfOther = 0;

		for(int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
		{
			Vec2 const &cornerOfThis = cornersOfThis[cornerIndex]; 
			Vec2 const &cornerOfOther = cornersOfOther[cornerIndex];

			float otherFromThis = thisPlane.GetDistance( cornerOfOther ); 
			float thisFromOther = otherPlane.GetDistance( cornerOfThis ); 

			
			if(cornerIndex == 0)
			{
				bestDistToA = otherFromThis;
				bestPointToA = cornerOfOther;
				bestThisPlane = thisPlane;

				bestSegmentThis = segmentsOfThis[planeIndex];
				bestSegmentOther = segmentsOfOther[planeIndex];

				worstCornerToPlane[planeIndex] = cornerOfOther;
				worstDistancesToThis[planeIndex] = otherFromThis;

				bestDistToB = thisFromOther;
				bestPointToB = cornerOfThis;
				bestOtherPlane = otherPlane;
				
				worstCornerToOtherPlane[planeIndex] = cornerOfThis;
				worstDistancesToOther[planeIndex] = thisFromOther;
			}

			//Update the feature point
			if(otherFromThis < bestDistToA)
			{
				bestDistToA = otherFromThis;
				bestPointToA = cornerOfOther;
				bestThisPlane = thisPlane;
				bestSegmentThis = segmentsOfThis[planeIndex];

				worstCornerToPlane[planeIndex] = cornerOfOther;
				worstDistancesToThis[planeIndex] = otherFromThis;
			}

			//Update the feature point for other plane
			if(thisFromOther < bestDistToB)
			{
				bestDistToB = thisFromOther;
				bestPointToB = cornerOfThis;
				bestOtherPlane = otherPlane;
				bestSegmentOther = segmentsOfOther[planeIndex];

				worstCornerToOtherPlane[planeIndex] = cornerOfThis;
				worstDistancesToOther[planeIndex] = thisFromOther;
			}

			inFrontOfThis += (otherFromThis >= 0.0f) ? 1 : 0; 
			inFrontOfOther += (thisFromOther >= 0.0f) ? 1 : 0; 
		}

		//We are not intersecting if there are exactly 4 in front of either plane. Early out bro
		if ((inFrontOfThis == 4) || (inFrontOfOther == 4)) {
			return false;
		}
	}

	float bestCaseThis = 0.f;
	float bestCaseOther = 0.f;
	int bestCaseIndexThis = 0;
	int bestCaseIndexOther = 0;

	Vec2 bestContactThis;
	Vec2 bestContactOther;

	for(int worstCaseIndex = 0; worstCaseIndex < 4; worstCaseIndex++)
	{
		if(worstCaseIndex == 0)
		{
			bestCaseThis = worstDistancesToThis[worstCaseIndex];
			bestCaseOther = worstDistancesToOther[worstCaseIndex];

			bestContactThis = worstCornerToPlane[worstCaseIndex];
			bestContactOther = worstCornerToOtherPlane[worstCaseIndex];
		}

		if(worstDistancesToThis[worstCaseIndex] > bestCaseThis)
		{
			bestCaseThis = worstDistancesToThis[worstCaseIndex];
			bestContactThis = worstCornerToPlane[worstCaseIndex];
			bestCaseIndexThis = worstCaseIndex;
		}

		if(worstDistancesToOther[worstCaseIndex] > bestCaseOther)
		{
			bestCaseOther = worstDistancesToOther[worstCaseIndex];
			bestContactOther = worstCornerToOtherPlane[worstCaseIndex];
			bestCaseIndexOther = worstCaseIndex;
		}

	}

	//Check which of the 2 are larger (smaller -ve number)
	if(bestCaseOther > bestCaseThis)
	{
		out->m_penetration = bestCaseOther * -1.f;
		
		//DEBUG
		DebugRenderOptionsT options;
		options.relativeCoordinates = true;
		options.space = DEBUG_RENDER_SCREEN;
		options.beginColor = Rgba::GREEN;
		g_debugRenderer->DebugRenderPoint2D(options, bestContactOther, 3.f);

		out->m_normal = planesOfOther[bestCaseIndexOther].m_normal;
	}
	else
	{
		out->m_penetration = bestCaseThis * -1.f;

		//DEBUG
		DebugRenderOptionsT options;
		options.relativeCoordinates = true;
		options.space = DEBUG_RENDER_SCREEN;
		g_debugRenderer->DebugRenderPoint2D(options, bestContactThis, 1.f);

		out->m_normal = planesOfThis[bestCaseIndexThis].m_normal * -1.f;
	}

	return true; 
}

bool GetManifold( Manifold2D *out, BoxCollider2D const &a, float aRadius, BoxCollider2D const &b, float bRadius )
{
	OBB2 boxA = a.GetWorldShape();
	OBB2 boxB = b.GetWorldShape();

	if (GetManifold( out, a, b )) {
		out->m_penetration += (aRadius + bRadius); 
		return true;
	}

	Segment2D sidesA[4];
	boxA.GetSides(sidesA); 
	Segment2D sidesB[4];
	boxB.GetSides(sidesB); 
	
	Vec2 cornersA[4];
	boxA.GetCorners(cornersA); 
	Vec2 cornersB[4];
	boxB.GetCorners(cornersB); 

	float bestMatchToA = 10000000; 
	float bestMatchToB = 10000000; 

	Vec2 bestA;
	Vec2 bestB;

	for(int sideIndex = 0; sideIndex < 4; sideIndex++)
	{
		Segment2D const &sideA = sidesA[sideIndex];
		Segment2D const &sideB = sidesB[sideIndex];

		for(int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
		{
			Vec2 const &cornerA = cornersA[cornerIndex];
			Vec2 const &cornerB = cornersB[cornerIndex];

			Vec2 closestA = sideA.GetClosestPoint(cornerB); 
			Vec2 closestB = sideB.GetClosestPoint(cornerA); 

			// distances are...
			// closestA to cornerB
			// closestB to cornerA
			float lengthSquared = (closestA - cornerB).GetLengthSquared() ;
			if (lengthSquared < bestMatchToA) {
				bestA = closestA; 
				bestB = cornerB; 
				bestMatchToA = lengthSquared;
			}

			lengthSquared = (closestB - cornerA).GetLengthSquared() ;
			if (lengthSquared < bestMatchToB) {
				bestA = cornerA; 
				bestB = closestB; 
				bestMatchToB = lengthSquared; 
			}
		}
	}


	// two closets points are bestA, bestB; 
	// normal is directoin between them;
	// penetration is sum of radius - distance; 

	float distance = GetDistance2D(bestA, bestB);
	if(distance > (aRadius + bRadius))
	{
		return false;
	}
	else
	{
		out->m_normal = bestB - bestA;
		out->m_penetration = aRadius + bRadius - distance;
		return true;
	}
}

bool GetManifold( Manifold2D *out, CapsuleCollider2D const &a, CapsuleCollider2D const &b )
{
	//Call the GetManifold with 2 BoxColliders and radius
	return GetManifold(out, a.GetWorldShape(), a.GetCapsuleRadius(), b.GetWorldShape(), b.GetCapsuleRadius());
}

bool GetManifold( Manifold2D *out, OBB2 const &a, float aRadius, OBB2 const &b, float bRadius )
{
	if (GetManifold( out, a, b )) 
	{
		out->m_penetration += (aRadius + bRadius); 
		return true;
	}

	Segment2D sidesA[4];
	a.GetSides(sidesA); 
	Segment2D sidesB[4];
	b.GetSides(sidesB); 

	Vec2 cornersA[4];
	a.GetCorners(cornersA); 
	Vec2 cornersB[4];
	b.GetCorners(cornersB); 

	float bestMatch = 10000000; 
	// float bestMatchToB = 10000000; 

	Vec2 bestA;
	Vec2 bestB;

	for(int sideIndex = 0; sideIndex < 4; sideIndex++)
	{
		Segment2D const &sideA = sidesA[sideIndex];
		Segment2D const &sideB = sidesB[sideIndex];

		for(int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
		{
			Vec2 const &cornerA = cornersA[cornerIndex];
			Vec2 const &cornerB = cornersB[cornerIndex];

			Vec2 closestA = sideA.GetClosestPoint(cornerB); 
			Vec2 closestB = sideB.GetClosestPoint(cornerA); 

			// distances are...
			// closestA to cornerB
			// closestB to cornerA
			float lengthSquared = (closestA - cornerB).GetLengthSquared() ;
			if (lengthSquared < bestMatch) {
				bestA = closestA; 
				bestB = cornerB; 
				bestMatch = lengthSquared;
			}

			lengthSquared = (closestB - cornerA).GetLengthSquared() ;
			if (lengthSquared < bestMatch) {
				bestA = cornerA; 
				bestB = closestB; 
				bestMatch = lengthSquared; 
			}
		}
	}


	// two closets points are bestA, bestB; 
	// normal is directoin between them;
	// penetration is sum of radius - distance; 

	float distance = GetDistance2D(bestA, bestB);
	if(distance > (aRadius + bRadius))
	{
		return false;
	}
	else
	{
		Vec2 disp = bestB - bestA;
		if(disp.GetLengthSquared() < 0.000001)
		{
			out->m_normal = Vec2(0.f, 1.f);
		}
		else
		{
			out->m_normal = disp.GetNormalized() * -1.f;
		}
		out->m_penetration = (aRadius + bRadius) - distance;
		return true;
	}
}

bool GetManifold( Manifold2D *out, BoxCollider2D const &a, CapsuleCollider2D const &b )
{
	return GetManifold(out, a.GetWorldShape(), 0.f, b.GetWorldShape(), b.GetCapsuleRadius());
}

bool GetManifold( Manifold2D *out, CapsuleCollider2D const &a, BoxCollider2D const &b )
{
	return GetManifold(out, a.GetWorldShape(), a.GetCapsuleRadius(), b.GetWorldShape(), 0.f);
}

bool IsDiscInBox(Manifold2D* out, const Vec2 &discCentre, const AABB2& boxShape, float radius)
{
	//Disc is either inside box or not colliding
	Vec2 horNormal;
	Vec2 vertNormal;
	Vec2 normal;
	float distance = 0.f;

	if(discCentre < boxShape.m_maxBounds && discCentre > boxShape.m_minBounds)
	{
		//Disc is inside the box
		float horizontalDist;
		float rightToDisc = boxShape.m_maxBounds.x - discCentre.x;
		float leftToDisc = discCentre.x - boxShape.m_minBounds.x;
		if( rightToDisc > leftToDisc )
		{
			horizontalDist = leftToDisc;
			horNormal = Vec2(-1.f, 0.f);
		}
		else
		{
			horizontalDist = rightToDisc;
			horNormal = Vec2(1.f, 0.f);
		}

		float vertDistance;
		float topToDisc = boxShape.m_maxBounds.y - discCentre.y;
		float botToDisc = discCentre.y - boxShape.m_minBounds.y;
		if( topToDisc > botToDisc )
		{
			vertDistance = botToDisc;
			vertNormal = Vec2(0.f, -1.f);
		}
		else
		{
			vertDistance = topToDisc;
			vertNormal = Vec2(0.f, 1.f);
		}

		//Figure which axis to push on
		if(horizontalDist > vertDistance)
		{
			normal = vertNormal;
			distance = vertDistance;
		}
		else
		{
			normal = horNormal;
			distance = horizontalDist;
		}

		out->m_normal = normal;
		out->m_penetration = distance + radius;
		return true;
	}
	else
	{
		//No collision
		return false;
	}
}

bool GetManifold( Manifold2D *out, Disc2DCollider const &discA, Disc2DCollider const &discB )
{
	float discARad = discA.GetWorldShape().GetRadius();
	float discBRad = discB.GetWorldShape().GetRadius();

	Vec2 discACenter = discA.GetWorldShape().GetCentre();
	Vec2 discBCenter = discB.GetWorldShape().GetCentre();
	float distanceSquared = GetDistanceSquared2D(discACenter, discBCenter);
	float radSumSquared = (discARad + discBRad) * (discARad + discBRad);

	if(distanceSquared < radSumSquared)
	{
		float distance = GetDistance2D(discACenter, discBCenter);
		Vec2 normal = discACenter - discBCenter;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = discARad + discBRad - distance;
		return true;
	}
	else
	{
		return false;
	}
}

void GenerateManifoldBoxToBox(Manifold2D* manifold, Vec2 const &min, Vec2 const &max)
{
	//Remember since we collided, the max is actually manifold min and min is actually manifold max
	float boxWidth = min.x - max.x;
	float boxHeight = min.y - max.y;
	float minValue = GetLowerValue(boxWidth, boxHeight);

	Vec2 normal;

	//Where is boxB wrt to boxA
	if(minValue == boxWidth)
	{
		//normal is along X
		normal = Vec2(1.f, 0.f);
	}
	else
	{
		//Normal is along Y
		normal = Vec2(0.f, 1.f);
	}

	manifold->m_normal = normal;
	manifold->m_penetration = minValue;
}

bool CheckAABB2ByAABB2(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//Check collision between 2 boxes
	AABB2Collider* boxA = reinterpret_cast<AABB2Collider*>(a);
	AABB2Collider* boxB = reinterpret_cast<AABB2Collider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *boxA, *boxB);

	if(result)
	{
		//Manifold is valid
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
	}
	else
	{
		//Invalid manifold as no collision
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
	}

	return result;
}

bool CheckAABB2ByDisc(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//box vs disc
	AABB2Collider* boxA = reinterpret_cast<AABB2Collider*>(a);
	Disc2DCollider* discB = reinterpret_cast<Disc2DCollider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *boxA, *discB);

	if(result)
	{
		out->m_manifold = manifold;
		out->m_Obj = a;
		out->m_otherObj = b;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
	}

	return result;
}

bool CheckDiscByDisc(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//disc vs disc
	Disc2DCollider* discA = reinterpret_cast<Disc2DCollider*>(a);
	Disc2DCollider* discB = reinterpret_cast<Disc2DCollider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *discA, *discB);

	if(result)
	{
		// out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_otherObj = nullptr;
		out->m_Obj = nullptr;
		return false;
	}
}

bool CheckDiscByAABB2(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//disc vs box
	Disc2DCollider* discA = reinterpret_cast<Disc2DCollider*>(a);
	AABB2Collider* boxB = reinterpret_cast<AABB2Collider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *discA, *boxB);

	if(result)
	{
		//out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
		return false;
	}
}

bool CheckOBB2ByOBB2( Collision2D* out, Collider2D* a, Collider2D* b )
{
	//OBB vs OBB
	BoxCollider2D* boxA = reinterpret_cast<BoxCollider2D*>(a);
	BoxCollider2D* boxB = reinterpret_cast<BoxCollider2D*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *boxA, *boxB);

	if(result)
	{
		//out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
		return false;
	}
}

bool CheckCapsuleByCapsule( Collision2D * out, Collider2D * a, Collider2D * b )
{
	//Capsule vs Capsule
	CapsuleCollider2D* capA = reinterpret_cast<CapsuleCollider2D*>(a);
	CapsuleCollider2D* capB = reinterpret_cast<CapsuleCollider2D*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *capA, *capB);

	if(result)
	{
		//out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
		return false;
	}
}

bool CheckCapsuleByOBB2( Collision2D* out, Collider2D* a, Collider2D* b )
{
	//Capsule vs OBB2
	CapsuleCollider2D* capA = reinterpret_cast<CapsuleCollider2D*>(a);
	BoxCollider2D* boxB = reinterpret_cast<BoxCollider2D*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *capA, *boxB);

	if(result)
	{
		//out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
		return false;
	}
}

bool CheckOBB2ByCapsule( Collision2D* out, Collider2D* a, Collider2D* b )
{
	//OBB2 vs Capsule
	BoxCollider2D* boxA = reinterpret_cast<BoxCollider2D*>(a);
	CapsuleCollider2D* capB = reinterpret_cast<CapsuleCollider2D*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *boxA, *capB);

	if(result)
	{
		//out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
		return false;
	}
}
