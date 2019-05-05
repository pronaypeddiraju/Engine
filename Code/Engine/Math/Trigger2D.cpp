//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Trigger2D.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/PhysicsTypes.hpp"
#include "Engine/Math/RigidBodyBucket.hpp"
#include "Engine/Math/TriggerBucket.hpp"
#include "Engine/Math/TriggerTouch2D.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Trigger2D::Trigger2D(PhysicsSystem* physicsSystem, eSimulationType simType)
{
	m_system = physicsSystem;
	m_simulationType = simType;
}

//------------------------------------------------------------------------------------------------------------------------------
Trigger2D::~Trigger2D()
{
	for (int triggerType = 0; triggerType < NUM_SIMULATION_TYPES; triggerType++)
	{
		int triggerCount = static_cast<int>(m_system->m_triggerBucket->m_triggerBucket[triggerType].size());

		for (int triggerIndex = 0; triggerIndex < triggerCount; triggerIndex++)
		{
			if (m_system->m_triggerBucket->m_triggerBucket[triggerType][triggerIndex] == this)
			{
				m_system->m_triggerBucket->m_triggerBucket[triggerType][triggerIndex] = nullptr;
			}
		}
	}

	delete m_collider;
	m_collider = nullptr;

}

//------------------------------------------------------------------------------------------------------------------------------
void Trigger2D::Update(uint frameNumber)
{
	RigidBodyBucket* bucket = g_physicsSystem->m_rbBucket;
	int numBodies = (int)bucket->m_RbBucket[DYNAMIC_SIMULATION].size();
	for (int colliderIndex = 0; colliderIndex < numBodies; colliderIndex++)
	{
		Rigidbody2D* rb = bucket->m_RbBucket[DYNAMIC_SIMULATION][colliderIndex];
		Collider2D* collider = bucket->m_RbBucket[DYNAMIC_SIMULATION][colliderIndex]->m_collider;

		//check condition where the other collider is nullptr
		if (collider == nullptr)
		{
			continue;
		}

		UpdateTouchesArray(rb, frameNumber);

		//UpdateExitObjects(rb, frameNumber);
	}
}

void Trigger2D::UpdateTouchesArray(Rigidbody2D* rb, uint frameNumber)
{
	Collision2D collision;
	if (rb->m_collider->IsTouching(&collision, m_collider))
	{
		//Check if the touch already exists in the vector
		int numTouches = (int)m_touches.size();
		bool colliderFound = false;
		for (int i = 0; i < numTouches; i++)
		{
			Collider2D* touchCollider = m_touches[i]->GetCollider();
			if (touchCollider == rb->m_collider)
			{
				m_touches[i]->SetCurrentFrame(frameNumber);
				colliderFound = true;
			}
		}

		if (colliderFound == false)
		{
			//Add the touch into the touches vector since it doesn't exist
			TriggerTouch2D* touch = new TriggerTouch2D(rb->m_collider, frameNumber);
			m_touches.push_back(touch);

			EventArgs args;
			g_eventSystem->FireEvent(m_onEnterEvent, args);
		}
	}
	else
	{
		//Check if the collider we haven't collided with exists in the vector
		for (int i = 0; i < (int)m_touches.size(); i++)
		{
			Collider2D* touchCollider = m_touches[i]->GetCollider();
			if (touchCollider == rb->m_collider && m_touches[i]->GetCurrentFrame() != m_touches[i]->GetEntryFrame())
			{
				//It exists in the array, so we should remove it and call the exit event on it
				rb->m_isAlive = false;
				
				//Remove it from the vector
				//m_touches[i]->Destroy();
				delete m_touches[i];
				m_touches[i] = nullptr;
				m_touches.erase(m_touches.begin() + i);
				i--;

				//Fire the exit event
				EventArgs args;
				g_eventSystem->FireEvent(m_onExitEvent, args);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Trigger2D::DebugRender(RenderContext* renderContext, const Rgba& color) const
{
	if (this == nullptr)
	{
		return;
	}

	eColliderType2D type = m_collider->GetType();

	std::vector<Vertex_PCU> verts;

	switch (type)
	{
	case COLLIDER_UNKOWN:
		break;
	case COLLIDER_AABB2:
	{
		AABB2Collider* collider = reinterpret_cast<AABB2Collider*>(m_collider);

		AddVertsForBoundingBox(verts, collider->GetWorldShape(), color, 0.5f);
	}
	break;
	case COLLIDER_DISC:
	{
		Disc2DCollider* collider = reinterpret_cast<Disc2DCollider*>(m_collider);

		AddVertsForRing2D(verts, collider->GetWorldShape().GetCentre(), collider->GetWorldShape().GetRadius(), 0.5f, color);
	}
	break;
	case COLLIDER_BOX:
	{
		BoxCollider2D* collider = reinterpret_cast<BoxCollider2D*>(m_collider);

		AddVertsForBoundingBox(verts, collider->GetWorldShape(), color, 0.5f);
	}
	break;
	case COLLIDER_CAPSULE:
	{
		CapsuleCollider2D* collider = reinterpret_cast<CapsuleCollider2D*>(m_collider);

		AddVertsForWireCapsule2D(verts, collider->GetWorldShape(), collider->GetCapsuleRadius(), color, 0.5f);
		AddVertsForLine2D(verts, collider->GetWorldShape().m_center, collider->GetWorldShape().m_center + collider->GetCapsuleRadius() * Vec2(0.f, 1.f).GetRotatedDegrees(m_transform.m_rotation), 0.2f, Rgba::WHITE);
	}
	break;
	case NUM_COLLIDER_TYPES:
		break;
	default:
		break;
	}

	renderContext->DrawVertexArray(verts);
}

//------------------------------------------------------------------------------------------------------------------------------
void Trigger2D::SetSimulationMode(eSimulationType simulationType)
{
	m_simulationType = simulationType;
}

//------------------------------------------------------------------------------------------------------------------------------
void Trigger2D::SetCollider(Collider2D* collider)
{
	m_collider = collider;
	m_collider->m_trigger = this;
}

void Trigger2D::SetTransform(const Transform2& transform)
{
	m_transform = transform;
}

//------------------------------------------------------------------------------------------------------------------------------
void Trigger2D::SetOnEnterEvent(const std::string& enterEventString)
{
	m_onEnterEvent = enterEventString;
}

//------------------------------------------------------------------------------------------------------------------------------
void Trigger2D::SetOnExitEvent(const std::string& exitEventString)
{
	m_onExitEvent = exitEventString;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Trigger2D::GetPosition() const
{
	return m_transform.m_position;
}

//------------------------------------------------------------------------------------------------------------------------------
eSimulationType Trigger2D::GetSimulationType()
{
	return m_simulationType;
}

