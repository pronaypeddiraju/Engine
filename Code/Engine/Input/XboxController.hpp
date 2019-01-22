#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Math/Vertex_PCU.hpp"

enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_DOWN,

	NUM_XBOX_BUTTONS	//Stores total number of buttons because enums are cool like that

};

class XboxController
{
	//Allows InputSystem to be my friend :')
	friend class InputSystem;	

public:
	explicit XboxController(int ControllerID);
	~XboxController();

	bool					IsConnected() const				{ return m_isConnected; }
	int						GetControllerID() const			{ return m_controllerID; }
	const AnalogJoyStick&   GetLeftJoystick() const			{ return m_leftJoystick; }
	const AnalogJoyStick&   GetRightJoystick() const		{ return m_rightJoystick; }
	float					GetLeftTrigger() const			{ return m_leftTriggerValue; }
	float					GetRightTrigger() const			{ return m_rightTriggerValue; }
	const KeyButtonState&   GetButtonState(XboxButtonID buttonID ) const;
	void					RenderDebugController() const;
	void					SetupDebugController();

	bool			m_debugMode = false;
private:
	void			Update();
	void			Reset();
	void			UpdateTrigger (float& triggerValue, unsigned char rawValue);
	void			UpdateJoystick (AnalogJoyStick& joystick, short rawX, short rawY);
	void			UpdateButton (XboxButtonID buttonID, unsigned short buttonFlags, unsigned short currentButtonFlag);

	void			CheckControllerState();
private:
	unsigned int ControllerID;
	const unsigned int MAX_CONTROLLERS = 4;

	const int				m_controllerID = -1;
	bool					m_isConnected = false;
	KeyButtonState			m_buttonStates[ NUM_XBOX_BUTTONS ];
	AnalogJoyStick			m_leftJoystick	= AnalogJoyStick(0.3f, 0.9f);
	AnalogJoyStick			m_rightJoystick = AnalogJoyStick(0.3f, 0.9f);
	float					m_leftTriggerValue = 0.f;
	float					m_rightTriggerValue = 0.f;

	float					m_outerDeadZoneRadius = 1.0f;
	float					m_correctedMaxRadius = 0.9f;
	float					m_innerDeadZoneRadius = 0.3f;
	Vertex_PCU				m_outerDeadZone[48];
	Vertex_PCU				m_correctedZone[48];
	Vertex_PCU				m_innerDeadZone[48];
	Vertex_PCU				m_rawPoint[48];
	Vertex_PCU				m_correctedPoint[48];
	int						m_NUM_DEBUG_VERTICES = 48;
};