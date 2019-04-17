//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <stdint.h>
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
class Clock
{
public:
	Clock();
	Clock( Clock* parent );
	~Clock();

	void				SetParent( Clock* parent);
	void				SetFrameLimit( double frameLimit );
						
	void				Step( float deltaSeconds );
	void				ForceStep( float deltaSeconds );
	void				Dilate( double timeDilation );
	void				Pause();
	void				Resume();
	void				ForcePause();
	void				ForceResume();

	inline double		GetFrameTime() { return m_frameTime; }
	bool				IsPaused();

private:
	double m_frameTime							= 0.0;
	double m_totalTime							= 0.0;
	uint16_t m_framCount						= 0;

	double m_frameLimit							= 0.1;
	double m_dilation							= 1.0;
	int m_pauseCount							= 0;

	Clock* m_parent								= nullptr;
	std::vector<Clock*> m_children;

public:
	static Clock m_masterClock;
};