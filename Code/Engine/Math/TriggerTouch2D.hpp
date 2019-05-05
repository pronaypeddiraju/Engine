//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------------------------
typedef unsigned int uint;
class Collider2D;

//------------------------------------------------------------------------------------------------------------------------------
class TriggerTouch2D
{
public:
	explicit TriggerTouch2D(Collider2D* collider, uint entryFrame);
	~TriggerTouch2D();

	inline void				SetCurrentFrame(uint frameNumber) { m_currentFrame = frameNumber; }

	inline Collider2D*		GetCollider() { return m_collider; }
	inline uint				GetCurrentFrame() { return m_currentFrame; }
	inline uint				GetEntryFrame() { return m_entryFrame; }

	void					Destroy();

private:
	Collider2D*		m_collider;
	uint			m_entryFrame;
	uint			m_currentFrame;
};