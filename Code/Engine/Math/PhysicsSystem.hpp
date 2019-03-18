#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Math/Rigidbody2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class RenderContext;
class Collider2D;
class RigidBodyBucket;

//------------------------------------------------------------------------------------------------------------------------------
class PhysicsSystem
{
	friend class Rigidbody2D;

public:
	PhysicsSystem();
	~PhysicsSystem();

	Rigidbody2D*			CreateRigidbody(eSimulationType simulationType);
	void					AddRigidbodyToVector( Rigidbody2D* rigidbody );
	void					DestroyRigidbody( Rigidbody2D* rigidbody );
	void					SetGravity(const Vec2& gravity);

	void					CopyTransformsFromObjects();
	void					CopyTransformsToObjects();
	void					Update(float deltaTime);
	void					SetAllCollisionsToFalse();

	void					DebugRender( RenderContext* renderContext ) const;

	const Vec2&				GetGravity() const;

private:

	void					RunStep(float deltaTime);

	void					MoveAllDynamicObjects(float deltaTime);
	void					CheckStaticVsStaticCollisions();
	void					ResolveDynamicVsStaticCollisions( bool canResolve );
	void					ResolveDynamicVsDynamicCollisions( bool canResolve );
public:

	//Way to store all rigidbodies
	//std::vector<Rigidbody2D*>		m_rigidbodyVector;
	RigidBodyBucket*				m_rbBucket;

	//system info like gravity
	Vec2							m_gravity = Vec2(0.0f, -9.8f);
};