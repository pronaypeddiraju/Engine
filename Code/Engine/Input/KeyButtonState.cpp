//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Input/KeyButtonState.hpp"

//------------------------------------------------------------------------------------------------------------------------------
void KeyButtonState::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = isNowPressed;
}

bool KeyButtonState::wasJustPressed() const
{
	if (m_isPressed && !m_wasPressedLastFrame)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool KeyButtonState::wasJustReleased() const
{
	if(!m_isPressed && m_wasPressedLastFrame)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void KeyButtonState::ResetButton()
{
	m_wasPressedLastFrame = false;
	m_isPressed = false;
}
