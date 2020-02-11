#include "Engine/Math/Ray3D.hpp"
#include "Engine/Math/Capsule3D.hpp"
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
Vec3 Ray3D::GetPointAtTime(float time) const
{
	Vec3 distanceInRayDirection = m_direction * time;
	return m_start + distanceInRayDirection;
}

//------------------------------------------------------------------------------------------------------------------------------
uint Raycast(float *out, const Ray3D& ray, Sphere const &sphere)
{
	//Calculate center between the 2 ray hit points on the sphere
	Vec3 pointToCenter = (sphere.m_point - ray.m_start);
	
	//Get the vector from ray start to hit center
	float projectedLengthToHitCenter = GetDotProduct(pointToCenter, ray.m_direction.GetNormalized());
	Vec3 hitCenter = projectedLengthToHitCenter * ray.m_direction + ray.m_start;

	float perpendicularToHitCenter = (hitCenter - sphere.m_point).GetLength();
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
	out[0] = (hits[0] - ray.m_start).GetLength();
	out[1] = (hits[1] - ray.m_start).GetLength();

	return 2U;
}

//------------------------------------------------------------------------------------------------------------------------------
uint Raycast(float *out, const Ray3D& ray, Plane3D const &plane)
{
	// plane equation : (p.n) - d = 0;
	// p = point on plane
	// n = plane normal
	// d = distance from origin

	// Ray equation : s + vt;
	// s = start point
	// v = direction of ray
	// t = time 

	// By substituting ray eqn in plane eqn we get
	// t = (d - s.n) / v.n

	//First early out is to check if the point p is already on the plane
	if ((GetDotProduct(ray.m_start, plane.m_normal) - plane.m_signedDistance) == 0)
	{
		out[0] = 0;
		return 1U;
	}

	float numerator = plane.m_signedDistance - GetDotProduct(ray.m_start, plane.m_normal);
	float denominator = GetDotProduct(ray.m_direction, plane.m_normal);

	if (denominator == 0)
	{
		//Ray is not hitting the plane (parallel)
		return 0U;
	}
	else
	{
		float t = numerator / denominator;
		if(t < 0)
		{
			return 0U;
		}
		else
		{
			out[0] = t;
			return 1U;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
uint Raycast(float *out, const Ray3D& ray, Capsule3D const &capsule)
{
	//------------------------------------------------------------------------------------------------------------------------------
	// Step 1: 
	// Solve for an infinite cylinder
	// | s + [(p-s).n]*n - p | = r
	// s = start point for infinite cylinder
	// p = hit point on infinite cylinder
	// r = radius
	// n = direction of cylinder (or normal of infinite cylinder)

	// Substituting ray eqn into the above equation, we get to this form (It's now a quadratic eqn)
	// t*t * (x . x) + t * (2 * x . y) + ((y . y) - (r * r)) = 0
	// x = (v . n)*n - v
	// y = s + (p.n)*n - (s.n)*n - p
	// v = ray direction

	Vec3 capsuleDirection = (capsule.m_end - capsule.m_start);
	capsuleDirection.Normalize();

	Vec3 x = GetDotProduct(ray.m_direction, capsuleDirection) * capsuleDirection - ray.m_direction;
	Vec3 y = capsule.m_start + (GetDotProduct(ray.m_start, capsuleDirection) * capsuleDirection)
	- GetDotProduct(capsule.m_start, capsuleDirection) * capsuleDirection - ray.m_start;

	// Now solving quadratic eqn for t with the following terms
	// a = (x . x)
	// b = (2 * x . y)
	// c = ((y . y) - (r * r))

	float a = GetDotProduct(x, x);
	float b = 2 * (GetDotProduct(x, y));

	float ca = GetDotProduct(y, y);
	float cb = (capsule.m_radius * capsule.m_radius);
	float c = (ca - cb);

	uint count = SolveQuadraticEquation(out, a, b, c);

	// We now have the 2 contact points (or no contact points) on the infinite cylinder
	if (count == 0)
	{

		return 0U;
	}

	//------------------------------------------------------------------------------------------------------------------------------
	// Step 2:
	// Verify that the points are in the finite cylinder
	// Get displacement from the contact points to the start and see if the dot product along direction is > 0
	//------------------------------------------------------------------------------------------------------------------------------
	Vec3 directionCap = capsuleDirection;
	
	float netOuts[6];
	int netOutIndex = 0;

	float sphereOut[2];

	for (int i = 0U; i < (int)count; i++)
	{
		Vec3 point = ray.GetPointAtTime(out[i]);
		Vec3 dispStart = capsule.m_start - point;
		Vec3 dispEnd = capsule.m_end - point;

		float resultStart = GetDotProduct(directionCap, dispStart);
		float resultEnd = GetDotProduct(directionCap, dispEnd);

		if (resultStart <= 0)
		{
			if (resultEnd <= 0)
			{
				//return 0U;
			}
			else
			{
				//We are within the finite capsule
				netOuts[netOutIndex] = out[i];
				netOutIndex++;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------
	// Step 3:
	// We can safely raycast to the sphere
	for (int i = 0; i < 2; i++)
	{
		int sphereHitCount = 0;
		Sphere* sphere = nullptr;
		if (i == 0)
		{
			sphere = new Sphere(capsule.m_start, capsule.m_radius);
		}
		else
		{
			sphere = new Sphere(capsule.m_end, capsule.m_radius);
		}

		sphereHitCount = Raycast(sphereOut, ray, *sphere);
		delete sphere;

		for (int j = 0; j < sphereHitCount; j++)
		{
			netOuts[netOutIndex] = sphereOut[j];
			netOutIndex++;
		}
	}


	//------------------------------------------------------------------------------------------------------------------------------
	// Step 4:
	// Return the smallest and largest values in sphereHitIndex
	float smallest = 100000.f;
	float largest = -100000.f;
	for (int i = 0U; i < (int)netOutIndex; i++)
	{
		smallest = GetLowerValue(smallest, netOuts[i]);
		largest = GetHigherValue(largest, netOuts[i]);
	}

	if (netOutIndex == 0U)
	{
		return 0U;
	}
	else
	{
		out[0] = smallest;
		out[1] = largest;
	}

	return 2U;
}
