//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include "Engine/Math/PhysicsTypes.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Rigidbody2D;

//------------------------------------------------------------------------------------------------------------------------------
class RigidBodyBucket
{
public:
	RigidBodyBucket();
	~RigidBodyBucket();

	std::vector<Rigidbody2D*>	m_RbBucket[NUM_SIMULATION_TYPES];
};