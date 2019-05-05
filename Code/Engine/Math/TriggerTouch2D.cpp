//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/TriggerTouch2D.hpp"
#include "Engine/Math/Collider2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
TriggerTouch2D::TriggerTouch2D(Collider2D* collider, uint entryFrame)
{
	m_collider = collider;
	m_entryFrame = entryFrame;
	m_currentFrame = entryFrame;
}

//------------------------------------------------------------------------------------------------------------------------------
TriggerTouch2D::~TriggerTouch2D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void TriggerTouch2D::Destroy()
{
	delete m_collider;
	m_collider = nullptr;
}

