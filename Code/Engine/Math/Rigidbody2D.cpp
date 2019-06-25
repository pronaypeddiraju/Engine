//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/RigidBodyBucket.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Rigidbody2D::Rigidbody2D(float mass)
{
	m_mass = mass;
}

//------------------------------------------------------------------------------------------------------------------------------
Rigidbody2D::Rigidbody2D( PhysicsSystem* physicsSystem, eSimulationType simulationType, float mass /*= 1.0f*/ )
{
	m_system = physicsSystem;
	m_simulationType = simulationType;
	m_mass = mass;
}

//------------------------------------------------------------------------------------------------------------------------------
Rigidbody2D::~Rigidbody2D()
{
	for(int rbType = 0; rbType < NUM_SIMULATION_TYPES; rbType++)
	{
		int rbCount = static_cast<int>(m_system->m_rbBucket->m_RbBucket[rbType].size());

		for(int rbIndex = 0; rbIndex < rbCount; rbIndex++)
		{
			if(m_system->m_rbBucket->m_RbBucket[rbType][rbIndex] == this)
			{
				m_system->m_rbBucket->m_RbBucket[rbType][rbIndex] = nullptr;
			}
		}
	}

	if (m_collider != nullptr)
	{
		delete m_collider;
		m_collider = nullptr;
	}

	//delete m_object_transform;
	//m_object_transform = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::Move( float deltaTime )
{
	if(m_simulationType == STATIC_SIMULATION)
	{
		return;
	}

	//Calc Acceleration due to gravity
	Vec2 gravity = m_system->GetGravity();
	Vec2 acc =  gravity * m_gravity_scale;

	//Apply the acceleration due to gravity
	m_velocity += acc * deltaTime;

	//Calc linear forces added
	acc = m_frameForces / m_mass;
	m_velocity += acc * deltaTime;

	m_velocity *= (1.0f - (GetLinearDrag() * deltaTime));

	//Set new position based on new velocities
	m_transform.m_position += m_velocity * deltaTime * Vec2(m_constraints.x, m_constraints.y);

	//Angular velocity steps
	float angularAcc = m_frameTorque / m_momentOfInertia;
	m_angularVelocity += angularAcc * deltaTime;
	m_angularVelocity *= (1.f - (GetAngularDrag() * deltaTime));
	m_rotation += m_angularVelocity * deltaTime * m_constraints.z;

	ApplyRotation();
}

//------------------------------------------------------------------------------------------------------------------------------
float Rigidbody2D::GetLinearDrag()
{
	return m_linearDrag;
}

//------------------------------------------------------------------------------------------------------------------------------
float Rigidbody2D::GetAngularDrag()
{
	return m_angularDrag;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::ApplyRotation()
{
	switch (m_collider->m_colliderType)
	{
	case COLLIDER_BOX:
	{
		BoxCollider2D* collider = reinterpret_cast<BoxCollider2D*>(m_collider);
		collider->m_localShape.SetRotation(m_rotation * m_constraints.z);
	}
	break;
	case COLLIDER_CAPSULE:
	{
		CapsuleCollider2D* collider = reinterpret_cast<CapsuleCollider2D*>(m_collider);
		collider->m_localShape.SetRotation(m_rotation * m_constraints.z);
	}
	break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::DebugRender( RenderContext* renderContext, const Rgba& color ) const
{
	if(this == nullptr)
	{
		return;
	}

	eColliderType2D type = m_collider->GetType();

	std::vector<Vertex_PCU> verts;

	switch( type )
	{
	case COLLIDER_UNKOWN:
		break;
	case COLLIDER_AABB2:
	{
		AABB2Collider* collider = reinterpret_cast<AABB2Collider*>(m_collider);

		AddVertsForBoundingBox(verts, collider->GetWorldShape(), color, 0.5f);
		break;
	}
	case COLLIDER_DISC:
	{
		Disc2DCollider* collider = reinterpret_cast<Disc2DCollider*>(m_collider);

		AddVertsForRing2D(verts, collider->GetWorldShape().GetCentre(), collider->GetWorldShape().GetRadius(), 0.5f, color);
		break;
	}
	case COLLIDER_BOX:
	{
		BoxCollider2D* collider = reinterpret_cast<BoxCollider2D*>(m_collider);

		AddVertsForBoundingBox(verts, collider->GetWorldShape(), color, 0.5f);
		break;
	}
	case COLLIDER_CAPSULE:
	{
		CapsuleCollider2D* collider = reinterpret_cast<CapsuleCollider2D*>(m_collider);

		AddVertsForWireCapsule2D(verts, collider->GetWorldShape(), collider->GetCapsuleRadius(), color, 0.5f);
		AddVertsForLine2D(verts, collider->GetWorldShape().m_center, collider->GetWorldShape().m_center + collider->GetCapsuleRadius() * Vec2(0.f, 1.f).GetRotatedDegrees(m_rotation), 0.2f, Rgba::WHITE);
		break;
	}
	case NUM_COLLIDER_TYPES:
		break;
	default:
		break;
	}

	renderContext->DrawVertexArray(verts);
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::SetSimulationMode( eSimulationType simulationType )
{
	m_simulationType = simulationType;
}

//------------------------------------------------------------------------------------------------------------------------------
Collider2D* Rigidbody2D::SetCollider( Collider2D* collider )
{
	m_collider = collider;
	return m_collider;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::SetObject( void* object, Transform2* objectTransform )
{
	m_object = object;
	m_object_transform = objectTransform;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::SetConstraints(const Vec3& constraints)
{
	m_constraints = constraints;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::SetConstraints(bool x, bool y, bool rotation)
{
	(x) ? m_constraints.x = 1.f : m_constraints.x = 0.f;
	(y) ? m_constraints.y = 1.f : m_constraints.y = 0.f;
	(rotation) ? m_constraints.z = 1.f : m_constraints.z = 0.f;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::Destroy()
{
	m_isAlive = false;
	m_collider->Destroy();
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Rigidbody2D::GetPosition() const
{
	return m_transform.m_position;
}

//------------------------------------------------------------------------------------------------------------------------------
eSimulationType Rigidbody2D::GetSimulationType()
{
	return m_simulationType;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::ApplyImpulses( Vec2 linearImpulse, float angularImpulse )
{
	m_velocity += linearImpulse / m_mass;
	m_velocity = m_velocity * Vec2(m_constraints.x, m_constraints.y);
	m_angularVelocity += RadiansToDegrees(angularImpulse / m_momentOfInertia); 
	m_angularVelocity *= m_constraints.z;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rigidbody2D::ApplyImpulseAt( Vec2 linearImpulse, Vec2 pointOfContact )
{
	Vec2 linear = linearImpulse; 

	Vec2 displacement = pointOfContact - GetPosition();  // world location of rigidbody (probably)
	Vec2 perpDisp = displacement.GetRotated90Degrees(); 
	float angular = GetDotProduct( linearImpulse, perpDisp ); 

	ApplyImpulses( linear, angular ); 
}
