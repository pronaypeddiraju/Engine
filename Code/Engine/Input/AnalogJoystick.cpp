#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"

AnalogJoyStick::AnalogJoyStick( float innerDeadZoneFraction, float outerDeadZoneFraction )
	:m_innerDeadZoneFraction (innerDeadZoneFraction),
	 m_outerDeadZoneFraction (outerDeadZoneFraction)
{
}

void AnalogJoyStick::Reset()
{
	m_rawPosition = Vec2(0.f, 0.f);
	m_correctedPosition = Vec2(0.f, 0.f);
	m_correctedDegrees = 0.f;
	m_correctedMagnitude = 0.f;
}

void AnalogJoyStick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	m_rawPosition.x = rawNormalizedX;
	m_rawPosition.y = rawNormalizedY;
	//now fix the raw values

	float rawVecLength = m_rawPosition.GetLength();
	
	if(rawVecLength < 0.3f)
	{
		m_correctedPosition = Vec2(0.f,0.f);
		m_correctedDegrees = 0.f;
		m_correctedMagnitude = 0.f;
	}
	else if (rawVecLength > 0.9f)
	{
		m_correctedPosition = m_rawPosition;
		m_correctedMagnitude = 1.0f;
		m_correctedPosition.SetLength(m_correctedMagnitude);
		m_correctedDegrees = m_correctedPosition.GetAngleDegrees();
	}
	else
	{
		m_correctedMagnitude = RangeMapFloat(rawVecLength, 0.3f, 0.9f, 0.f, 1.f);
		m_correctedPosition = m_rawPosition;
		m_correctedPosition.SetLength(m_correctedMagnitude);
		m_correctedDegrees = m_correctedPosition.GetAngleDegrees();
	}
}

