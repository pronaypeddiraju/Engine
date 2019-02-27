#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/AABB2.hpp"
#include <stdio.h>

//180.f/PI to degrees per radian

//Converts radians to degrees
float RadiansToDegrees( float Radians )
{
	return (Radians * (180.0f / PI));
}

//Converts degrees to radians
float DegreesToRadians( float Degrees )
{
	return (Degrees * (PI / 180.0f));
}

//Returns cos of degrees
float CosDegrees( float Degrees )
{
	//Returns Cos of degrees
	return cosf(Degrees * (PI / 180.0f));
}

//Returns sin of degrees
float SinDegrees( float Degrees )
{
	//Returns sin of degrees
	return sinf(Degrees * (PI / 180.0f));
}

//Returns conversion of an ATan of a vector to degrees
float ATan2Degrees( float y, float x )
{
	return atan2f(y ,x) * (180.0f / PI);
}

float GetAngularDisplacement( float angleA, float angleB )
{
	return (angleB - angleA);
}

//Function to get distance between 2 points in 2D
float GetDistance2D( const Vec2& pointA, const Vec2& pointB )
{
	float xDisp = (pointA.x - pointB.x);
	float yDisp = (pointA.y - pointB.y);
	return sqrtf((xDisp * xDisp) + (yDisp * yDisp));
}

//Function to get distance between 2 points in 3D
float GetDistance3D( const Vec3& pointA, const Vec3& pointB )
{
	float xDisp = (pointA.x - pointB.x);
	float yDisp = (pointA.y - pointB.y);
	float zDisp = (pointA.z - pointB.z);
	return sqrtf((xDisp * xDisp) + (yDisp * yDisp) + (zDisp * zDisp));
}

//Function to get distance between X and Y coordinates of 2 points
float GetDistanceXY( const Vec3& pointA, const Vec3& pointB )
{
	float xDisp = (pointA.x - pointB.x);
	float yDisp = (pointA.y - pointB.y);
	return sqrtf((xDisp * xDisp) + (yDisp * yDisp));
}

//Function to get square of distance between 2 points (Faster computation)
float GetDistanceSquared2D(const Vec2& pointA,const Vec2& pointB )
{
	float xDisp = (pointA.x - pointB.x);
	float yDisp = (pointA.y - pointB.y);
	return (xDisp * xDisp) + (yDisp * yDisp);
}

//Function to get square of distance between 2 points in 3D
float GetDistanceSquared3D( const Vec3& pointA, const Vec3& pointB )
{
	float xDisp = (pointA.x - pointB.x);
	float yDisp = (pointA.y - pointB.y);
	float zDisp = (pointA.z - pointB.z);
	return (xDisp * xDisp) + (yDisp * yDisp) + (zDisp * zDisp);
}

//Function to get square of distance between 2 points (only X and Y)
float GetDistanceSquaredXY( const Vec3& pointA, const Vec3& pointB )
{
	float xDisp = (pointA.x - pointB.x);
	float yDisp = (pointA.y - pointB.y);
	return (xDisp * xDisp) + (yDisp * yDisp);
}

//Function to check if discs overlap
bool DoDiscsOverlap( const Vec2& centreA, float radiusA, const Vec2& centreB, float radiusB )
{
	float discDistance = GetDistance2D(centreA, centreB);
	float sumRadii = radiusA + radiusB;
	if(discDistance < sumRadii)
		return true;
	else
		return false;
}

//Function to check if spheres overlap
bool DoSpheresOverlap( const Vec3 & centreA, float radiusA, const Vec3 & centreB, float radiusB )
{
	float discDistance = GetDistance3D(centreA, centreB);
	float sumRadii = radiusA + radiusB;
	if(discDistance < sumRadii)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------------------------
//Is point inside disc 2D
//------------------------------------------------------------------------------------------------------------------------------
bool IsPointInDisc2D( const Vec2& point, const Vec2& centre, float radius )
{
	//check if the point is inside the disc
	Vec2 distVector = point - centre;
	float distToVector = distVector.GetLength();

	if(distToVector * distToVector < radius * radius)
		return true;
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//Push discs out of each other if they overlap
//------------------------------------------------------------------------------------------------------------------------------
void PushDiscsIfOverlapping(Vec2 &discAPos, float discAradius, Vec2 &discBPos, float discBradius )
{
	//Check if overlapping
	if(!DoDiscsOverlap(discAPos, discAradius, discBPos, discBradius))
		return;

	//The discs are overlapping
	PushDiscsApart(discAPos, discAradius, discBPos, discBradius);
}

//------------------------------------------------------------------------------------------------------------------------------
//Logic to push discs apart from each other
//------------------------------------------------------------------------------------------------------------------------------
void PushDiscsApart(Vec2& positionA, float radiusA, Vec2& positionB, float radiusB)
{
	//Push disc 1 out of disc 2
	Vec2 distVecBetweenPoints = positionA - positionB;
	float distBetweenPoints = distVecBetweenPoints.GetLength();

	//Get the normal to push out to
	Vec2 normalForDiscA = distVecBetweenPoints.GetNormalized();
	Vec2 normalForDiscB = normalForDiscA * -1.f;

	//Set the normal Vectors to the length we want to push by
	float distToPush = radiusA + radiusB - distBetweenPoints;
	normalForDiscA.SetLength(distToPush/2);
	normalForDiscB.SetLength(distToPush/2);

	positionA += normalForDiscA;
	positionB += normalForDiscB;
}

//------------------------------------------------------------------------------------------------------------------------------
//Is point witing a sector
//------------------------------------------------------------------------------------------------------------------------------
bool IsPointInSector2D( const Vec2& point, const Vec2& origin, float orientationDegrees, float maxDistance, float apertureDegrees )
{
	//Check if the point is in the disc of the sector angle
	if(!IsPointInDisc2D(point, origin, maxDistance))
		return false;

	//If we get here, we're on the disk
	//Now check if we are indeed on the sector
	Vec2 displacementVector = (origin - point);
	float angleToPoint = displacementVector.GetAngleDegrees();
	float angleDispToPoint = GetAngularDisplacement(orientationDegrees, angleToPoint);
	return (fabsf(angleDispToPoint) <= (apertureDegrees * 0.5f));
}

//------------------------------------------------------------------------------------------------------------------------------
//Push disc out of a box
//------------------------------------------------------------------------------------------------------------------------------
void PushDiscOutOfAABB2(Vec2& position, float radius, const AABB2& tileBounds )
{
	//Get the closest point on the AABB2
	Vec2 closestPoint = GetClosestPointOnAABB2(position, tileBounds);
	PushDiscOutOfPoint(position, radius, closestPoint);
}

//------------------------------------------------------------------------------------------------------------------------------
//Get the closest point on a box
//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetClosestPointOnAABB2(const Vec2& position, const AABB2& box)
{
	Vec2 p;
	p.x = Clamp(position.x, box.m_minBounds.x, box.m_maxBounds.x);
	p.y = Clamp(position.y, box.m_minBounds.y, box.m_maxBounds.y);
	return p;
}

//------------------------------------------------------------------------------------------------------------------------------
//Get the closest point on a disc
//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetClosestPointOnDisc2D( const Vec2& referencePos, const Vec2& discCenter, float discRadius )
{
	//Get displacement vector from disc centre to point
	Vec2 displacement = referencePos - discCenter;

	if(displacement.GetLengthSquared() < discRadius*discRadius)
	{
		return referencePos;
	}

	//Set the length to radius
	displacement.SetLength(discRadius);

	//Add disc centre to it to get the closest point on disc
	Vec2 closestPoint = displacement + discCenter;
	return closestPoint;
}

/*
bool DoesDiscOverlapLine2D( const Vec2& discCenter, float discRadius, const Vec2& pointOnLine, const Vec2& anotherPointOnLine )
{
	this needs some code
}
*/

//------------------------------------------------------------------------------------------------------------------------------
//Get the closest point on the line 
//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetClosestPointOnLine2D( const Vec2& referencePos, const Vec2& pointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2 dispToPoint1 = referencePos - pointOnLine;
	Vec2 dispToPoint2 = referencePos - anotherPointOnLine;
	Vec2 dispBetweenPoints = pointOnLine - anotherPointOnLine;
	Vec2 projection = Vec2::ZERO;
	Vec2 closestPoint = Vec2::ZERO;

	if(dispToPoint1.GetLengthSquared() < dispToPoint2.GetLengthSquared())
	{
		//pointOnLine is closest point
		projection = GetProjectedVector(dispToPoint1, dispBetweenPoints);
	}
	else
	{
		//anotherPointOnLine is closest point
		projection = GetProjectedVector(dispToPoint2, dispBetweenPoints);
	}

	closestPoint = pointOnLine + projection;

	return closestPoint;
}

//------------------------------------------------------------------------------------------------------------------------------
//Get the closest point on the line segment
//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetClosestPointOnLineSegment2D( const Vec2& referencePos, const Vec2& lineStart, const Vec2& lineEnd )
{
	Vec2 lineSegment = lineEnd - lineStart;
	Vec2 vectorToStartPoint = referencePos - lineStart;
	Vec2 vectorToEndPoint = referencePos - lineEnd;

	if(GetDotProduct(lineSegment, vectorToStartPoint) < 0)
	{
		//We are in Voronoi region 1, point is before line segment start
		return lineStart;
	}
	else if(GetDotProduct(lineSegment, vectorToEndPoint))
	{
		//We are in Voronoi region 2, point is after line segment start
		return lineEnd;
	}
	else
	{
		Vec2 closestPoint = GetClosestPointOnLine2D(referencePos, vectorToStartPoint, vectorToEndPoint);
		return closestPoint;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//Get closest point on a capusle
//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetClosestPointOnCapsule2D( const Vec2& referencePos, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius )
{
	//Check if you're on either end of the capsule
	Vec2 point = GetClosestPointOnLineSegment2D(referencePos, capsuleStart, capsuleEnd);

	if(point == capsuleStart)
	{
		//are we in the first hemisphere?
		point = GetClosestPointOnDisc2D(referencePos, capsuleStart, capsuleRadius);
		return point;
	}
	else if(point == capsuleEnd)
	{
		//Are we in the last hemisphere?
		point = GetClosestPointOnDisc2D(referencePos, capsuleEnd, capsuleRadius);
		return point;
	}
	else
	{
		//We are in the middle region
		point = GetClosestPointOnLine2D(referencePos, capsuleStart, capsuleEnd);
		Vec2 displacement = referencePos - point;
		if(displacement.GetLengthSquared() < capsuleRadius*capsuleRadius)
		{
			//We are inside the capsule
			return referencePos;
		}
		else
		{
			//We are outside the capsule
			Vec2 normal = referencePos - point;
			normal.Normalize();
			//Get the vector to be added
			Vec2 vectorToAdd = normal * capsuleRadius;
			point += vectorToAdd;
			return point;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
float GetRayImpactFractionVsDisc2D( const Vec2& rayStart, const Vec2& rayNormalizedDir, float rayLength, const Vec2& discCenter, float discRadius )
{
	//Get the end point using the ray direction
	Vec2 rayEnd = rayStart + rayNormalizedDir * rayLength;

	//Get the ray
	Vec2 ray = rayEnd - rayStart;

	//Get displacement to centre from start
	Vec2 dispStartToCentre = discCenter - rayStart;

	//Get the projection of that displacement on the ray
	float projectionOnRay = GetProjectedLength(dispStartToCentre, rayNormalizedDir);
	Vec2 projectionVectorOnRay = projectionOnRay * rayNormalizedDir;

	//Get closest point to disc centre
	Vec2 closestToCentre = rayStart + projectionVectorOnRay;

	//Get the ray's tangent vector
	Vec2 rayTangent = rayNormalizedDir;
	rayTangent.Rotate90Degrees();
	
	//Get the projected length on tangent
	float projectionOnTangent = GetProjectedLength(dispStartToCentre, rayTangent);

	float radiusSqr = discRadius*discRadius;
	float projectionSqr = projectionOnTangent* projectionOnTangent;
	float impactPointToCentreNormal = 0;

	if(projectionSqr < radiusSqr)
	{
		//Get the point using Pythagoras theorem
		impactPointToCentreNormal = static_cast<float>(sqrt(radiusSqr - projectionSqr));
	}
	else
	{
		return 0;
	}

	float impactDist = projectionOnRay - impactPointToCentreNormal;
	impactDist = Clamp(impactDist, 0, rayLength);
	return impactDist;
}

//------------------------------------------------------------------------------------------------------------------------------
bool IsPointInCapsule2D( const Vec2& point, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius )
{
	Vec2 closestPoint = GetClosestPointOnCapsule2D(point, capsuleStart, capsuleEnd, capsuleRadius);
	if(closestPoint == point)
		return true;
	else
	{
		return false;
	}
}

Vec3 GetSphericalCoordinates( float radius, float angleTheta, float anglePhi )
{
	Vec3 coordinates;

	coordinates.x = CosDegrees(anglePhi) * CosDegrees(angleTheta);
	coordinates.y = SinDegrees(anglePhi);
	coordinates.z = CosDegrees(anglePhi) * SinDegrees(angleTheta);

	coordinates *= radius;
	return coordinates;
}

//------------------------------------------------------------------------------------------------------------------------------
void PushDiscOutOfPoint(Vec2& discPosition, float radius, const Vec2& pushOutOfPoint)
{
	if(!IsPointInDisc2D(pushOutOfPoint, discPosition, radius))
		return;

	//We are definitely in the disc
	float overlapDist = GetOverlapDistance(pushOutOfPoint, discPosition, radius);
	Vec2 moveDisp = discPosition - pushOutOfPoint;
	moveDisp.SetLength(overlapDist);
	discPosition = discPosition - moveDisp;
}

//------------------------------------------------------------------------------------------------------------------------------
float GetOverlapDistance(Vec2 point, Vec2 centre, float radius)
{
	Vec2 pointToCentre = point - centre;
	float displacement = pointToCentre.GetLength();
	return (displacement - radius);
}

//------------------------------------------------------------------------------------------------------------------------------
bool IsPointInSector2D( const Vec2& point, const Vec2& origin, const Vec2& orientationVector, float maxDistance, float apertureDegrees )
{
	float orientationAngle = orientationVector.GetAngleDegrees();
	return IsPointInSector2D(point, origin, orientationAngle, maxDistance, apertureDegrees);
}

//------------------------------------------------------------------------------------------------------------------------------
float GetDotProduct( const Vec2& vecA, const Vec2& vecB )
{
	return (vecA.x * vecB.x + vecA.y * vecB.y);
}

float GetDotProduct( const Vec3& vecA, const Vec3& vecB )
{
	return (vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z);
}

float GetDotProduct( const Vec4& vecA, const Vec4& vecB )
{
	return (vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z + vecA.w * vecB.w);
}

//------------------------------------------------------------------------------------------------------------------------------
float GetProjectedLength( const Vec2& vecToProject, const Vec2& vecToProjectOnto )
{
	Vec2 basisDirection = vecToProjectOnto.GetNormalized();
	return GetProjectedLengthOnAxis2D( vecToProject, basisDirection );
}

//------------------------------------------------------------------------------------------------------------------------------
float GetProjectedLengthOnAxis2D( const Vec2& vecToProject, const Vec2& normalizedAxis )
{
	return GetDotProduct(vecToProject, normalizedAxis);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetProjectedVector( const Vec2& vecToProject, const Vec2& vecToProjectOnto)
{
	Vec2 basisDirection = vecToProjectOnto.GetNormalized();
	return GetProjectedVectorOnAxis2D(vecToProject, basisDirection);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetProjectedVectorOnAxis2D( const Vec2& vecToProject, const Vec2& axis)
{
	Vec2 projectionVector = axis * GetDotProduct(vecToProject, axis);
	return projectionVector;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetReflectedVector(const Vec2& vecToReflect, const Vec2& vecToReflectBasedOn)
{
	//Get the normal vector
	Vec2 normal = vecToReflectBasedOn.GetNormalized();

	return ReflectVectorOffSurfaceNormal(vecToReflect, normal);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 ReflectVectorOffSurfaceNormal( const Vec2& vecToReflect, const Vec2& normalToReflectOffOf )
{
	Vec2 vecAlongNormal = GetDotProduct(normalToReflectOffOf, vecToReflect) * normalToReflectOffOf;

	//Get the tangent vector
	Vec2 vecAlongSurface = vecToReflect - vecAlongNormal;

	//Inverted normal
	vecAlongNormal *= -1.0f;

	return Vec2(vecAlongSurface + vecAlongNormal);
}

//Move a 2D position vector
//------------------------------------------------------------------------------------------------------------------------------
Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY )
{
	Vec2 tempPos = position;

	//Scaling
	tempPos.x = tempPos.x * uniformScale;
	tempPos.y = tempPos.y * uniformScale;

	//Orientation
	tempPos = tempPos.GetRotatedDegrees(rotationDegreesOnZ);
	
	//Translation
	tempPos += Vec2(translateXY.x, translateXY.y);
	
	return tempPos;
}

//Move a 3D vector
//------------------------------------------------------------------------------------------------------------------------------
Vec3 TransformPosition3D( const Vec3& position, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY )
{
	Vec3 tempPos = position;
	float zValue = position.z;

	//Scaling
	tempPos.x = tempPos.x * uniformScale;
	tempPos.y = tempPos.y * uniformScale;
	
	//Orientation
	tempPos = tempPos.GetRotatedAboutZDegrees(rotationDegreesOnZ);
	
	//Translation
	tempPos += Vec3(translateXY.x, translateXY.y, 0.f);
	tempPos.z = zValue;

	return tempPos;
}

//Convert to a range
//------------------------------------------------------------------------------------------------------------------------------
float RangeMapFloat( float inputValue, float inRangeStart, float inRangeEnd, float outRangeStart, float outRangeEnd)
{
	if(inRangeEnd == inRangeStart)
		return 0.5f * (outRangeStart + outRangeEnd);

	float inRange = inRangeEnd - inRangeStart;
	float outRange = outRangeEnd - outRangeStart;
	float inRangeValue = inputValue - inRangeStart;

	float normalizedValue = inRangeValue / inRange;

	float outRangeValue = normalizedValue * outRange;
	float outValue = outRangeStart + outRangeValue;
	
	return outValue;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetVectorFromAngle( const float angle )
{
	float radianAngle = DegreesToRadians(angle);
	float newX = cosf(radianAngle);
	float newY = sinf(radianAngle);
	return Vec2(newX, newY);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetVectorFromAngleRadians(const float radianAngle)
{
	float newX = cosf(radianAngle);
	float newY = sinf(radianAngle);
	return Vec2(newX, newY);
}

//------------------------------------------------------------------------------------------------------------------------------
bool IsBitFlagSet( unsigned short buttonFlags, unsigned short buttonFlag )
{
	return (buttonFlags & buttonFlag) != 0;
}

//------------------------------------------------------------------------------------------------------------------------------
float GetHigherValue(const float a, const float b)
{
	if(a > b)
		return a;
	else
	{
		return b;
	}
}



//------------------------------------------------------------------------------------------------------------------------------
float Clamp( float x, float minClamp, float maxClamp )
{
	if(x < minClamp)
		x = minClamp;
	else if(x > maxClamp)
		x = maxClamp;

	return x;
}

//Method to get angular displacement
//------------------------------------------------------------------------------------------------------------------------------
float GetAngluarDisplacement(float startDegrees, float endDegrees)
{
	float angularDisp = endDegrees - startDegrees;
	while( angularDisp > 180.f )
	{
		angularDisp -= 360.f;
	}
	while( angularDisp < -180.f )
	{
		angularDisp += 360.f;
	}

	return angularDisp;

}

//------------------------------------------------------------------------------------------------------------------------------
float GetTurnedToward(float currentDegrees, float goalDegrees, float maxPositiveDeltaDegrees)
{
	float angularDisplacement = GetAngluarDisplacement(currentDegrees, goalDegrees);
	if(fabsf(angularDisplacement) < maxPositiveDeltaDegrees)
	{
		return goalDegrees;
	}

	if(angularDisplacement > 0)
		return currentDegrees + maxPositiveDeltaDegrees;
	else
		return currentDegrees - maxPositiveDeltaDegrees;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetLocalVectorFromWorld2D( const Vec2& worldVector, const Vec2& iBasis, const Vec2& jBasis )
{
	//We are assuming iBasis and jBasis are normalized
	//Get projection on i basis
	float projectionOni = GetDotProduct(worldVector, iBasis);
	Vec2 projectedVectorOni = iBasis;
	projectedVectorOni.SetLength(projectionOni);

	//Get projection on j basis
	float projectionOnj = GetDotProduct(worldVector, jBasis);
	Vec2 projectedVectorOnj = jBasis;
	projectedVectorOnj.SetLength(projectionOnj);

	return (projectedVectorOni + projectedVectorOnj);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetLocalPositionFromWorld2D( const Vec2& worldPosition, const Vec2& iBasis, const Vec2& jBasis, const Vec2& tBasis )
{
	//Get the displacement vector between position and local origin
	Vec2 displacementFromTBasis = worldPosition - tBasis;
	
	//Get projection of the displacement vector over i and j basis
	float projectionOni = GetDotProduct(displacementFromTBasis, iBasis);
	float projectionOnj = GetDotProduct(displacementFromTBasis, jBasis);

	return Vec2(projectionOni, projectionOnj);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetWorldVectorFromLocal2D( const Vec2& localVector, const Vec2& iBasis, const Vec2& jBasis )
{
	Vec2 worldVector = localVector.x * iBasis + localVector.y * jBasis;

	return worldVector;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GetWorldPositionFromWorld2D( const Vec2& localPosition, const Vec2& iBasis, const Vec2& jBasis, const Vec2& tBasis )
{
	Vec2 worldPosition = tBasis + ((localPosition.x * iBasis) + (localPosition.y * jBasis));

	return worldPosition;
}

float SmoothStart2( float inputZeroToOne )
{
	float t2 = inputZeroToOne * inputZeroToOne;
	return t2;
}

float SmoothStart3( float inputZeroToOne )
{
	float t3 = inputZeroToOne * inputZeroToOne * inputZeroToOne;
	return t3;
}

float SmoothStart4( float inputZeroToOne )
{
	float t4 = inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne;
	return t4;
}

float SmoothStart5( float inputZeroToOne )
{
	float t5 = inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne;
	return t5;
}

float SmoothStop2( float inputZeroToOne )
{
	float var = 1.0f - inputZeroToOne;
	return 1 - (var * var) ;
}

float SmoothStop3( float inputZeroToOne )
{
	float var = 1.0f - inputZeroToOne;
	return 1 - (var * var * var) ;
}

float SmoothStop4( float inputZeroToOne )
{
	float var = 1.0f - inputZeroToOne;
	return 1 - (var * var * var * var) ;
}

float SmoothStop5( float inputZeroToOne )
{
	float var = 1.0f - inputZeroToOne;
	return 1 - (var * var * var * var * var) ;
}

float SmoothStep3( float inputZeroToOne )
{
	float t = inputZeroToOne; 
	return (3.f * t * t * t) - (2.f * t * t);
}

float SmoothStep5( float inputZeroToOne )
{
	float t = inputZeroToOne;
	float t3 = t * t * t;
	return  (t3 * t * t * 6) - (15 * t3 * t) + (10 * t3);
}