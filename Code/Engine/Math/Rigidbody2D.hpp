#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Transform2.hpp"
#include "Engine/Math/Vec3.hpp"

class Collider2D;
class RenderContext;
struct Rgba;

//------------------------------------------------------------------------------------------------------------------------------
enum eSimulationType
{
	TYPE_UNKOWN = -1,

	STATIC_SIMULATION,
	DYNAMIC_SIMULATION,

	NUM_SIMULATION_TYPES
};

//------------------------------------------------------------------------------------------------------------------------------
struct PhysicsMaterialT
{
	float restitution = 1.f;
};

//------------------------------------------------------------------------------------------------------------------------------
class Rigidbody2D
{
public:
	Rigidbody2D( float mass = 1.0f);
	explicit Rigidbody2D(PhysicsSystem* physicsSystem, eSimulationType simulationType, float mass = 1.0f);
	~Rigidbody2D();

	//Apply a single step of movement
	void									Move(float deltaTime);
	void									ApplyRotation();
	//Apply specific movement
	inline void								MoveBy(Vec2 movement) { m_transform.m_position += movement * Vec2(m_constraints.x, m_constraints.y);}
	
	//Impulses
	void									ApplyImpulses(Vec2 linearImpulse, float angularImpulse);
	void									ApplyImpulseAt(Vec2 linearImpulse, Vec2 pointOfContact);
	
	//Forces and Torques
	inline void								AddForce(Vec2 force) { m_frameForces += force; }
	inline void								AddTorque(float torque) { m_frameTorque += torque; }

	//Render
	void									DebugRender(RenderContext* renderContext, const Rgba& color) const;
	
	//Mutators
	void									SetSimulationMode(eSimulationType simulationType);
	Collider2D*								SetCollider(Collider2D* collider);
	void									SetObject(void* object, Transform2* objectTransform);
	void									SetConstraints(const Vec3& constraints);
	void									SetConstraints(bool x, bool y, bool rotation);

	//Accessors
	Vec2									GetPosition() const;
	eSimulationType							GetSimulationType();
	float									GetLinearDrag();
	float									GetAngularDrag();

public:
	PhysicsSystem*							m_system = nullptr; 			// system this rigidbody belongs to; 
	void*									m_object = nullptr; 			// user (game) pointer for external use
	Transform2*								m_object_transform = nullptr;	// what does this rigidbody affect

	Transform2								m_transform;					// rigidbody transform (mimics the object at start of frame, and used to tell the change to object at end of frame)
	Vec2									m_gravity_scale = Vec2::ONE;	// how much are we affected by gravity
	Vec2									m_velocity = Vec2::ZERO; 
	float 									m_angularVelocity = 0.f;
	float									m_mass;  						// how heavy am I

	Collider2D*								m_collider = nullptr;			// my shape; (could eventually be made a set)
	bool									m_isTrigger = false;
	PhysicsMaterialT						m_material;

	float									m_momentOfInertia = 0.f;
	float									m_rotation = 0.f;

	Vec2									m_frameForces = Vec2::ZERO;
	float									m_frameTorque = 0.f;

	float									m_friction = 1.f;				// Friction along the surface

	float									m_linearDrag = 0.1f;
	float									m_angularDrag = 0.1f;

	Vec3									m_constraints = Vec3(0.f, 1.f, 0.f);

private:
	eSimulationType							m_simulationType = TYPE_UNKOWN;

};