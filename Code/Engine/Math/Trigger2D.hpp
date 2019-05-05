#pragma once
//------------------------------------------------------------------------------------------------------------------------------
// Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/Transform2.hpp"
#include "Engine/Math/Vec3.hpp"

class Collider2D;
class RenderContext;
class TriggerTouch2D;
struct Rgba;

//------------------------------------------------------------------------------------------------------------------------------
class Trigger2D
{
public:
	Trigger2D(PhysicsSystem* physicsSystem, eSimulationType simType);
	~Trigger2D();

	//Update
	void									Update(uint frameNumber);
	void									UpdateTouchesArray(Rigidbody2D* rb, uint frameNumber);

	//Render
	void									DebugRender(RenderContext* renderContext, const Rgba& color) const;

	//Mutators
	void									SetSimulationMode(eSimulationType simulationType);
	void									SetCollider(Collider2D* collider);
	void									SetTransform(const Transform2& transform);
	void									SetOnEnterEvent(const std::string& enterEventString);
	void									SetOnExitEvent(const std::string& exitEventString);

	//Accessors
	Vec2									GetPosition() const;
	eSimulationType							GetSimulationType();
	
public:
	PhysicsSystem*							m_system = nullptr; 			// system this trigger belongs to; 
	
	Transform2								m_transform;					// trigger transform
	Collider2D*								m_collider = nullptr;			// my shape; (could eventually be made a set)

	std::string								m_onEnterEvent = "";
	std::string								m_onExitEvent = "";

private:
	eSimulationType							m_simulationType = TYPE_UNKOWN;
	std::vector<TriggerTouch2D*>			m_touches;
};