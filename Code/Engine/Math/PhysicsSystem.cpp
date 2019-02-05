//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/Collider2D.hpp"

PhysicsSystem* g_physicsSystem = nullptr;

PhysicsSystem::PhysicsSystem()
{

}

PhysicsSystem::~PhysicsSystem()
{

}

Rigidbody2D* PhysicsSystem::CreateRigidbody()
{
	Rigidbody2D *rigidbody = new Rigidbody2D();
	return rigidbody;
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

void PhysicsSystem::BeginFrame()
{
	// reset per frame stuff; 
	// copy all transforms over; 
}

void PhysicsSystem::PreRender()
{
	// figure out movement, apply to actual game object;

	// whatever else you may want to do end frame; 
}

void PhysicsSystem::Update( float deltaTime )
{
	BeginFrame(); 
	// debug: clear all frame information
	// such as if they are currently touching another object; 

	// we'll eventually switch to fixed-step updates, so we'll call down immediately to a run_step to make
	// that port easier; 
	RunStep( deltaTime );

	PreRender();  
}

void PhysicsSystem::EndFrame()
{

}

void PhysicsSystem::DebugRender( RenderContext* renderContext ) const
{
	//
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
		m_rigidbodyVector[objectIndex]->Move(deltaTime);
	}

	//Correct all objects
	int numColliders = static_cast<int>(m_colliderVector.size());
	for(int colliderIndex = 0; colliderIndex < numColliders; colliderIndex++)
	{
		for(int otherColliderIndex = 0; otherColliderIndex < numColliders; otherColliderIndex++)
		{
			//Make sure we aren't the same collider
			if(m_colliderVector[otherColliderIndex] == m_colliderVector[colliderIndex])
			{
				continue;
			}

			if(m_colliderVector[colliderIndex]->IsTouching(m_colliderVector[otherColliderIndex]))
			{
				//Set collision to true
				m_colliderVector[colliderIndex]->SetCollision(true);
				m_colliderVector[otherColliderIndex]->SetCollision(true);
			}
		}
	}
}

