#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Transform2.hpp"

class Collider2D;
class RenderContext;
struct Rgba;

enum eSimulationType
{
	TYPE_UNKOWN = -1,

	STATIC_SIMULATION,
	DYNAMIC_SIMULATION,

	NUM_SIMULATION_TYPES
};

class Rigidbody2D
{
public:
	Rigidbody2D( float mass = 1.0f);
	~Rigidbody2D();

	//Apply a single step of movement
	void			Move(float deltaTime);

	void			DebugRender(RenderContext* renderContext, const Rgba& color) const;
	
	//Mutators
	void			SetSimulationMode(eSimulationType simulationType);
	Collider2D*		SetCollider(Collider2D* collider);
	void			SetObject(void* object, Transform2* objectTransform);

	//Accessors
	Vec2			GetPosition() const;

public :
	PhysicsSystem*							m_system = nullptr; 			// system this rigidbody belongs to; 
	void*									m_object = nullptr; 			// user (game) pointer for external use
	Transform2*								m_object_transform = nullptr;	// what does this rigidbody affect

	Transform2								m_transform;					// rigidbody transform (mimics the object at start of frame, and used to tell the change to object at end of frame)
	Vec2									m_gravity_scale = Vec2::ONE;	// how much are we affected by gravity
	Vec2									m_velocity; 
	float									m_mass;  						// how heavy am I

	Collider2D*								m_collider;						// my shape; (could eventually be made a set)

private:
	eSimulationType							m_simulationType = TYPE_UNKOWN;

};