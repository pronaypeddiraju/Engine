#pragma once

class KeyButtonState
{
public:
	void UpdateStatus (bool isNowPressed);
	bool IsPressed() const				{return m_isPressed;}
	bool wasJustPressed() const;
	bool wasJustReleased() const;
	void ResetButton();

private:
	bool m_isPressed	=	false;
	bool m_wasPressedLastFrame = false;
};
