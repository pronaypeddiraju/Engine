//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/PhysicsSystem.hpp"

Rigidbody2D::Rigidbody2D(float mass)
{
	m_mass = mass;
}

Rigidbody2D::~Rigidbody2D()
{

}

void Rigidbody2D::Move( float deltaTime )
{
	//Calc Acceleration
	Vec2 gravity = m_system->GetGravity();
	Vec2 acc =  gravity * m_gravity_scale;

	//Apply the acceleration
	m_velocity += acc * deltaTime;
	m_transform.m_position += m_velocity * deltaTime;
}

void Rigidbody2D::SetSimulationMode( eSimulationType simulationType )
{
	m_simulationType = simulationType;
}

Collider2D* Rigidbody2D::SetCollider( Collider2D* collider )
{
	m_collider = collider;
	return m_collider;
}

void Rigidbody2D::SetObject( void* object, Transform2* objectTransform )
{
	m_object = object;
	m_object_transform = objectTransform;
}

Vec2 Rigidbody2D::GetPosition() const
{
	return m_transform.m_position;
}

