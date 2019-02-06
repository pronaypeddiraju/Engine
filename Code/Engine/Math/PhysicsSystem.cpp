//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Renderer/Rgba.hpp"

PhysicsSystem* g_physicsSystem = nullptr;

PhysicsSystem::PhysicsSystem()
{

}

PhysicsSystem::~PhysicsSystem()
{

}


Rigidbody2D* PhysicsSystem::CreateRigidbody( eSimulationType simulationType )
{
	Rigidbody2D *rigidbody = new Rigidbody2D(this, simulationType);
	return rigidbody;
}

void PhysicsSystem::AddRigidbodyToVector(Rigidbody2D* rigidbody)
{
	m_rigidbodyVector.push_back(rigidbody);
}

void PhysicsSystem::DestroyRigidbody( Rigidbody2D* rigidbody )
{
	delete rigidbody;
	rigidbody = nullptr;
}

void PhysicsSystem::SetGravity( const Vec2& gravity )
{
	m_gravity = gravity;
}

void PhysicsSystem::CopyTransformsFromObjects()
{
	// reset per frame stuff; 
	// copy all transforms over;
	int numRigidbodies = static_cast<int>(m_rigidbodyVector.size());

	for(int rigidbodyIndex = 0; rigidbodyIndex < numRigidbodies; rigidbodyIndex++)
	{
		if(m_rigidbodyVector[rigidbodyIndex] != nullptr)
		{
			m_rigidbodyVector[rigidbodyIndex]->m_transform = *m_rigidbodyVector[rigidbodyIndex]->m_object_transform;
		}
	}
}

void PhysicsSystem::CopyTransformsToObjects()
{
	// figure out movement, apply to actual game object;
	int numRigidbodies = static_cast<int>(m_rigidbodyVector.size());

	for(int rigidbodyIndex = 0; rigidbodyIndex < numRigidbodies; rigidbodyIndex++)
	{
		if(m_rigidbodyVector[rigidbodyIndex] != nullptr)
		{
			*m_rigidbodyVector[rigidbodyIndex]->m_object_transform = m_rigidbodyVector[rigidbodyIndex]->m_transform;
		}
	}
}

void PhysicsSystem::Update( float deltaTime )
{
	CopyTransformsFromObjects(); 
	// debug: clear all frame information
	// such as if they are currently touching another object; 
	SetAllCollisionsToFalse();

	// we'll eventually switch to fixed-step updates, so we'll call down immediately to a run_step to make
	// that port easier; 
	RunStep( deltaTime );

	CopyTransformsToObjects();  
}

void PhysicsSystem::SetAllCollisionsToFalse()
{
	int numRigidbodies = static_cast<int>(m_rigidbodyVector.size());

	for(int rbIndex = 0; rbIndex < numRigidbodies; rbIndex++)
	{
		if(m_rigidbodyVector[rbIndex] != nullptr)
		{
			m_rigidbodyVector[rbIndex]->m_collider->SetCollision(false);
		}
	}
}

void PhysicsSystem::DebugRender( RenderContext* renderContext ) const
{
	int numRigidbodies = static_cast<int>(m_rigidbodyVector.size());
	for(int rbIndex = 0; rbIndex < numRigidbodies; rbIndex++)
	{
		//Nullptr check first
		if(m_rigidbodyVector[rbIndex] == nullptr)
		{
			continue;
		}

		eSimulationType simType = m_rigidbodyVector[rbIndex]->GetSimulationType();
	
		switch( simType )
		{
		case TYPE_UNKOWN:
		break;
		case STATIC_SIMULATION:
		{
			if(m_rigidbodyVector[rbIndex]->m_collider->m_inCollision)
			{
				m_rigidbodyVector[rbIndex]->DebugRender(renderContext, Rgba::MAGENTA);
			}
			else
			{
				m_rigidbodyVector[rbIndex]->DebugRender(renderContext, Rgba::YELLOW);
			}
		}
		break;
		case DYNAMIC_SIMULATION:
		{
			if(m_rigidbodyVector[rbIndex]->m_collider->m_inCollision)
			{
				m_rigidbodyVector[rbIndex]->DebugRender(renderContext, Rgba::RED);
			}
			else
			{
				m_rigidbodyVector[rbIndex]->DebugRender(renderContext, Rgba::BLUE);
			}
		}
		break;
		case NUM_SIMULATION_TYPES:
		break;
		default:
		break;
		}
	}
}

const Vec2& PhysicsSystem::GetGravity() const
{
	return m_gravity;
}

void PhysicsSystem::RunStep( float deltaTime )
{
	//First move all rigidbodies
	int numObjects = static_cast<int>(m_rigidbodyVector.size());
	for (int objectIndex = 0; objectIndex < numObjects; objectIndex++)
	{
		if(m_rigidbodyVector[objectIndex] != nullptr)
		{
			m_rigidbodyVector[objectIndex]->Move(deltaTime);
		}
	}

	//Set colliding or not colliding here
	for(int colliderIndex = 0; colliderIndex < numObjects; colliderIndex++)
	{
		if(m_rigidbodyVector[colliderIndex] == nullptr)
		{
			continue;
		}

		for(int otherColliderIndex = 0; otherColliderIndex < numObjects; otherColliderIndex++)
		{
			//check condition where the other collider is nullptr
			if(m_rigidbodyVector[otherColliderIndex] == nullptr)
			{
				continue;
			}

			//Make sure we aren't the same rigidbody
			if(m_rigidbodyVector[colliderIndex] == m_rigidbodyVector[otherColliderIndex])
			{
				continue;
			}
						
			if(m_rigidbodyVector[colliderIndex]->m_collider->IsTouching(m_rigidbodyVector[otherColliderIndex]->m_collider))
			{
				//Set collision to true
				m_rigidbodyVector[colliderIndex]->m_collider->SetCollision(true);
				m_rigidbodyVector[otherColliderIndex]->m_collider->SetCollision(true);
			}
		}
	}
}

