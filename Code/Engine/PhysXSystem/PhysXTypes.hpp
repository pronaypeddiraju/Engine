#pragma once
#include "Engine/Math/Vec3.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum PhysXConvexMeshCookingTypes_T
{
	QUICKHULL
};

//------------------------------------------------------------------------------------------------------------------------------
enum PhysXGeometryTypes_T
{
	BOX,
	CAPSULE,
	SPHERE,
	CONVEX_HULL
};

//------------------------------------------------------------------------------------------------------------------------------
enum PhysXJointType_T
{
	SIMPLE_SPHERICAL,
	LIMITED_SPHERICAL,
	SIMPLE_FIXED,
	BREAKABLE_FIXED,
	D6_DAMPED,
	D6_SIMPLE
};

//------------------------------------------------------------------------------------------------------------------------------
struct PhysXVehicleBaseAttributes
{
	float m_chassisMass = 1500.0f;
	Vec3 m_chassisDims = Vec3(2.5f, 1.20f, 5.0f);
	Vec3 m_chassisMOI = Vec3((m_chassisDims.y*m_chassisDims.y + m_chassisDims.z*m_chassisDims.z)*m_chassisMass / 12.0f,
		(m_chassisDims.x*m_chassisDims.x + m_chassisDims.z*m_chassisDims.z)*0.8f*m_chassisMass / 12.0f,
		(m_chassisDims.x*m_chassisDims.x + m_chassisDims.y*m_chassisDims.y)*m_chassisMass / 12.0f);

	Vec3 m_chassisCMOffset = Vec3(0.0f, -m_chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	float m_wheelMass = 20.0f;
	float m_wheelRadius = 0.5f;
	float m_wheelWidth = 0.4f;
	float m_wheelMOI = 0.5f*m_wheelMass*m_wheelRadius*m_wheelRadius;
	uint m_nbWheels = 4;

	void	SetChassisMOI()
	{
		m_chassisMOI = Vec3((m_chassisDims.y*m_chassisDims.y + m_chassisDims.z*m_chassisDims.z)*m_chassisMass / 12.0f,
			(m_chassisDims.x*m_chassisDims.x + m_chassisDims.z*m_chassisDims.z)*0.8f*m_chassisMass / 12.0f,
			(m_chassisDims.x*m_chassisDims.x + m_chassisDims.y*m_chassisDims.y)*m_chassisMass / 12.0f);
	}

	void	SetChassisCMOffset()
	{
		m_chassisCMOffset = Vec3(0.0f, -m_chassisDims.y*0.5f + 0.65f, 0.25f);
	}

	void	SetWheelMOI()
	{
		m_wheelMOI = 0.5f*m_wheelMass*m_wheelRadius*m_wheelRadius;
	}
};