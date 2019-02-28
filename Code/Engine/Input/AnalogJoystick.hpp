//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"

class AnalogJoyStick
{
	//This allows XboxController to access my functions
	friend class XboxController;

public:
	explicit AnalogJoyStick( float innerDeadZoneFraction, float outerDeadZoneFraction);

	Vec2				GetPosition() const						{ return m_correctedPosition; }
	float				GetMagnitude() const					{ return m_correctedMagnitude; }
	float				GetAngleDegrees() const					{ return m_correctedDegrees; }
	Vec2				GetRawPosition() const					{ return m_rawPosition; }
	float				GetInnerDeadZoneFraction() const		{ return m_innerDeadZoneFraction; }
	float				GetOuterDeadZoneFraction() const		{ return m_outerDeadZoneFraction; }
	void				Reset();

protected:
	void				UpdatePosition( float rawNormalizedX, float rawNormalizedY );

private:
	const float			m_innerDeadZoneFraction;
	const float			m_outerDeadZoneFraction;
	Vec2				m_rawPosition			= Vec2(0.f, 0.f);
	Vec2				m_correctedPosition		= Vec2(0.f, 0.f);
	float				m_correctedDegrees		= 0.f;
	float				m_correctedMagnitude	= 0.f;
};