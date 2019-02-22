//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Rigidbody2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class RigidBodyBucket
{
public:
	RigidBodyBucket();
	~RigidBodyBucket();

	std::vector<Rigidbody2D*>	m_RbBucket[NUM_SIMULATION_TYPES];
};