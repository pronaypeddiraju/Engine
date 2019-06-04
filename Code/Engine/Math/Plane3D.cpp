#include "Engine/Math/Plane3D.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Plane3D::Plane3D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Plane3D::Plane3D(Vec3 normal, float signedDistance)
{
	m_normal = normal;
	m_signedDistance = signedDistance;
}

//------------------------------------------------------------------------------------------------------------------------------
Plane3D::~Plane3D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Plane3D Plane3D::MakeFromTriangleLHRule(const Vec3& point1, const Vec3& point2, const Vec3& point3) 
{
	//Create 2 vectors from a point to the others
	Vec3 dir1 = point2 - point1;
	Vec3 dir2 = point3 - point1;

	//Get cross product between them as plane normal
	Vec3 normal = GetCrossProduct(dir1, dir2);
	normal.Normalize();

	float signedDistance = GetDistance3D(point1, Vec3::ZERO);
	return Plane3D(normal, signedDistance);
}

