#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"

class Rigidbody2D;
class RenderContext;
class Collider2D;

class PhysicsSystem
{
	friend class Rigidbody2D;

public:
	PhysicsSystem();
	~PhysicsSystem();

	Rigidbody2D*			CreateRigidbody();
	void					DestroyRigidbody(Rigidbody2D* rigidbody);
	void					SetGravity(const Vec2& gravity);

	void					BeginFrame();
	void					PreRender();
	void					Update(float deltaTime);
	void					EndFrame();

	void					DebugRender(RenderContext* renderContext) const;

	//Accessors
	const Vec2&				GetGravity() const;

private:
	void					AddCollider(Collider2D* collider);
	void					RemoveCollider(Collider2D* collider);

	void					RunStep(float deltaTime);

public:

	//Way to store all rigidbodies
	std::vector<Rigidbody2D*>		m_rigidbodyVector;
	//Way to store all colliders
	std::vector<Collider2D*>		m_colliderVector;

	//system info like gravity
	Vec2							m_gravity = Vec2(0.0f, -9.8f);
};