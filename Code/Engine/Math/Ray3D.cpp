#include "Engine/Math/Ray3D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/Sphere.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Ray3D::Ray3D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Ray3D::Ray3D(Vec3 start, Vec3 direction)
{
	m_start = start;
	m_direction = direction;
}

//------------------------------------------------------------------------------------------------------------------------------
Ray3D::~Ray3D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Vec3 Ray3D::GetPointAtTime(float time)
{
	Vec3 distanceInRayDirection = m_direction * time;
	return m_start + distanceInRayDirection;
}

//------------------------------------------------------------------------------------------------------------------------------
uint Raycast(float *out, Ray3D ray, Sphere const &sphere)
{
	//Calculate center between the 2 ray hit points on the sphere
	Vec3 pointToCenter = (sphere.m_point - ray.m_start);
	
	//Get the vector from ray start to hit center
	float projectedLengthToHitCenter = GetDotProduct(pointToCenter, ray.m_direction.GetNormalized());
	Vec3 hitCenter = projectedLengthToHitCenter * ray.m_direction + ray.m_start;

	float perpendicularToHitCenter = pointToCenter.GetLength();
	float hitCenterToPointSquared = (sphere.m_radius * sphere.m_radius - perpendicularToHitCenter * perpendicularToHitCenter);

	if (hitCenterToPointSquared <= 0.0000001f)
	{
		return 0U;
	}

	float hitCenterToHitPoint = sqrt(hitCenterToPointSquared);

	Vec3 hits[2];
	hits[0] = hitCenter - hitCenterToHitPoint * ray.m_direction;
	hits[1] = hitCenter + hitCenterToHitPoint * ray.m_direction;

	//out = new float[2];
	out[0] = (hits[0] - hitCenter).GetLength();
	out[1] = (hits[1] - hitCenter).GetLength();

	return 2U;
}

//------------------------------------------------------------------------------------------------------------------------------
uint Raycast(float *out, Ray3D ray, Plane3D const &plane)
{
	// plane equation : (p.n) - d = 0;
	// p = point on plane
	// n = plane normal
	// d = distance from origin

	// Ray equation : p + vt;
	// p = start point
	// v = direction of ray
	// t = time 

	// By substituting ray eqn in plane eqn we get
	// t = (d - p.n) / v.n

	float numerator = plane.m_signedDistance - GetDotProduct(ray.m_start, plane.m_normal);
	float denominator = GetDotProduct(ray.m_direction, plane.m_normal);

	if (denominator == 0)
	{
		//Ray is not hitting the plane (parallel)
		return 0U;
	}
	else
	{
		out[0] = numerator / denominator;
		return 1U;
	}
}
