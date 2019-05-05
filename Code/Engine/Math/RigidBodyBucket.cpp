//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/RigidBodyBucket.hpp"
#include "Engine/Math/Rigidbody2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
RigidBodyBucket::RigidBodyBucket()
{

}

//------------------------------------------------------------------------------------------------------------------------------
RigidBodyBucket::~RigidBodyBucket()
{
	for(int numTypes = 0; numTypes < NUM_SIMULATION_TYPES; numTypes++)
	{
		int numBodies = static_cast<int>(m_RbBucket[numTypes].size());
		for(int rbIndex = 0; rbIndex < numBodies; rbIndex++)
		{
			delete m_RbBucket[numTypes][rbIndex];
			m_RbBucket[numTypes][rbIndex] = nullptr;
		}
	}
}

