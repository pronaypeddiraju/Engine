//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vertex_PCU.hpp"
#include <math.h>
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);

struct Vec2;
struct Vec3;
struct Vec4;
struct AABB2;

constexpr float PI = 3.1415926535897932384626433832795f;

//------------------------------------------------------------------------------------------------------------------------------
//Angle utilities:
float			RadiansToDegrees(float Radians);
float			DegreesToRadians(float Degrees);
float			CosDegrees(float Degrees);
float			SinDegrees(float Degrees);
float			ATan2Degrees(float y, float x);
float			GetAngularDisplacement(float angleA, float angleB);
Vec2			GetVectorFromAngle(const float angle);
Vec2			GetVectorFromAngleRadians(const float radianAngle);

//Geometric Utilities
float			GetDistance2D(const Vec2& pointA, const Vec2& pointB);
float			GetDistance3D(const Vec3& pointA, const Vec3& pointB);
float			GetDistanceXY(const Vec3& pointA, const Vec3& pointB);
float			GetDistanceSquared2D(const Vec2& pointA, const Vec2& pointB);
float			GetDistanceSquared3D(const Vec3& pointA, const Vec3& pointB);
float			GetDistanceSquaredXY(const Vec3& pointA, const Vec3& pointB);

bool			DoDiscsOverlap(const Vec2& centreA, float radiusA, const Vec2& centreB, float radiusB);
float			GetOverlapDistance(Vec2 point, Vec2 centre, float radius);
bool			DoSpheresOverlap(const Vec3& centreA, float radiusA, const Vec3& centreB, float radiusB);

bool			IsPointInDisc2D(const Vec2& point, const Vec2& centre, float radius);
Vec2			GetRandomPointOnDisc2D(const Vec2& center, float radius, float minDegrees, float maxDegrees, float& angleUsed);
bool			IsPointInSector2D(const Vec2& point, const Vec2& origin, float orientationDegrees, float maxDistance, float apertureDegrees);
bool			IsPointInSector2D(const Vec2& point, const Vec2& origin, const Vec2& orientationVector, float maxDistance, float apertureDegrees);
bool			IsPointInAABBB2(const AABB2& box, const Vec2& point);

float			GetDotProduct(const Vec2& vecA, const Vec2& vecB);
float			GetDotProduct(const Vec3& vecA, const Vec3& vecB);
float			GetDotProduct(const Vec4& vecA, const Vec4& vecB);

Vec3			GetCrossProduct(const Vec3& vecA, const Vec3& vecB);

float			GetProjectedLength(const Vec2& vecToProject, const Vec2& vecToProjectOnto);
Vec2			GetProjectedVector(const Vec2& vecToProject, const Vec2& vecToProjectOnto);
Vec2			GetReflectedVector(const Vec2& vecToReflect, const Vec2& vecToReflectBasedOn);
float			GetProjectedLengthOnAxis2D(const Vec2& vecToProject, const Vec2& normalizedAxis);
Vec2			GetProjectedVectorOnAxis2D(const Vec2& vecToProject, const Vec2& axis);
Vec2			ReflectVectorOffSurfaceNormal( const Vec2& incomingVector, const Vec2& surfaceNormal );

Vec2			GetClosestPointOnAABB2(const Vec2& position, const AABB2& box);
Vec2			GetClosestPointOnDisc2D( const Vec2& referencePos, const Vec2& discCenter, float discRadius );
Vec2			GetClosestPointOnLine2D( const Vec2& referencePos, const Vec2& pointOnLine, const Vec2& anotherPointOnLine );
Vec2			GetClosestPointOnLineSegment2D( const Vec2& referencePos, const Vec2& lineStart, const Vec2& lineEnd );
Vec2			GetClosestPointOnCapsule2D( const Vec2& referencePos, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius );
float			GetRayImpactFractionVsDisc2D( const Vec2& rayStart, const Vec2& rayNormalizedDir, float rayLength, const Vec2& discCenter, float discRadius );
bool			IsPointInCapsule2D( const Vec2& point, const Vec2& capsuleStart, const Vec2& capsuleEnd, float capsuleRadius );

bool			IsPointOnLineSegment2D(const Vec2& position, const Vec2& lineStart, const Vec2& lineEnd, float epsilon = 0.001f);

Vec3			GetSphericalToCartesian(float radius, float angleTheta, float anglePhi);

std::vector<Vec2>			GenerateConvexPolygonFromDisc2D(int discRadius);

//Transform Utilities
Vec2			TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY);
Vec3			TransformPosition3D( const Vec3& position, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY);

void			PushDiscsIfOverlapping(Vec2 &discAPos, float discAradius, Vec2 &discBPos, float discBradius );
void			PushDiscsApart(Vec2& positionA, float radiusA, Vec2& positionB, float radiusB);
void			PushDiscOutOfDisc(Vec2& positionA, float radiusA, Vec2& positionB, float radiusB);
void			PushDiscOutOfAABB2(Vec2& position, float radius, const AABB2& tileBounds );
void			PushDiscOutOfPoint(Vec2& discPosition, float radius, const Vec2& pushOutOfPoint);

//Range map function
float			RangeMapFloat(float inputValue, float inRangeMin, float inRangeMax, float outRangeMin, float outRangeMax);

//Bitwise functions
bool			IsBitFlagSet( unsigned short buttonFlags, unsigned short buttonFlag );

//Other utilities
float			GetHigherValue(const float a, const float b);
float			Clamp(float x, float minClamp, float maxClamp);
float			GetAngluarDisplacement(float startDegrees, float endDegrees);
float			GetTurnedToward(float currentDegrees, float goalDegrees, float maxPositiveDeltaDegrees);

//World space and Local space conversion functions
Vec2			GetLocalVectorFromWorld2D(const Vec2& worldVector, const Vec2& iBasis, const Vec2& jBasis);
Vec2			GetLocalPositionFromWorld2D(const Vec2& worldPosition, const Vec2& iBasis, const Vec2& jBasis, const Vec2& tBasis);

Vec2			GetWorldVectorFromLocal2D(const Vec2& localVector, const Vec2& iBasis, const Vec2& jBasis);
Vec2			GetWorldPositionFromWorld2D(const Vec2& localPosition, const Vec2& iBasis, const Vec2& jBasis, const Vec2& tBasis);

//Equation solvers
uint			SolveQuadraticEquation(float* out, float a, float b, float c);

//Smoothing function
float			SmoothStart2(float inputZeroToOne);
float			SmoothStart3(float inputZeroToOne);
float			SmoothStart4(float inputZeroToOne);
float			SmoothStart5(float inputZeroToOne);

float			SmoothStop2(float inputZeroToOne);
float			SmoothStop3(float inputZeroToOne);
float			SmoothStop4(float inputZeroToOne);
float			SmoothStop5(float inputZeroToOne);

float			SmoothStep3(float inputZeroToOne);
float			SmoothStep5( float inputZeroToOne );

constexpr float fSQRT_3_OVER_3 = 0.5773502691896257645091f; 

//------------------------------------------------------------------------------------------------------------------------------
template< typename T >
T GetLowerValue( const T& a, const T& b )
{
	if(a < b)
		return a;
	else
		return b;
}