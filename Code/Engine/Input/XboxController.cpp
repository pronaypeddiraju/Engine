//------------------------------------------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_renderContext;

XINPUT_STATE g_xboxControllerState;

//------------------------------------------------------------------------------------------------------------------------------
XboxController::XboxController( int ControllerID )
	: m_controllerID(ControllerID)
{

}

//------------------------------------------------------------------------------------------------------------------------------
XboxController::~XboxController()
{

}

//------------------------------------------------------------------------------------------------------------------------------
const KeyButtonState& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

//------------------------------------------------------------------------------------------------------------------------------
void XboxController::Update()
{
	memset( &g_xboxControllerState, 0, sizeof( g_xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerID, &g_xboxControllerState );
	if( errorStatus != ERROR_SUCCESS )
	{
		m_isConnected = false;
		//Condition when xbox controller is not connected
		Reset();
	}
	else
	{
		//Your xbox controller is connected
		//Check what the values of your buttons are
		m_isConnected = true;
		//check the word here
		UpdateButton(XBOX_BUTTON_ID_A, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_ID_B, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_ID_X, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_ID_Y, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_Y);
		UpdateButton(XBOX_BUTTON_ID_BACK, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_ID_START, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_START);
		UpdateButton(XBOX_BUTTON_ID_LSHOULDER, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_RSHOULDER, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_LTHUMB, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_RTHUMB, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_DPAD_RIGHT, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_UP, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_ID_DPAD_LEFT, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_DOWN, g_xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);


		//Store the left and right trigger values
		UpdateTrigger(m_leftTriggerValue, g_xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTriggerValue, g_xboxControllerState.Gamepad.bRightTrigger);
		//Update the analog sticks (left and right)
		UpdateJoystick(m_leftJoystick, g_xboxControllerState.Gamepad.sThumbLX, g_xboxControllerState.Gamepad.sThumbLY);
		UpdateJoystick(m_rightJoystick, g_xboxControllerState.Gamepad.sThumbRX, g_xboxControllerState.Gamepad.sThumbRY);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void XboxController::Reset()
{
	//Reset the controller values;
	m_leftTriggerValue = 0.f;
	m_rightTriggerValue = 0.f;

	for(int j = 0; j < NUM_XBOX_BUTTONS; j++)
	{
		//Reset buttons here
		m_buttonStates[static_cast<XboxButtonID>(j)].ResetButton();
	}

	//Update the analog sticks (left and right)
	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
	//throw std::logic_error( "The method or operation is not implemented." );
}

//------------------------------------------------------------------------------------------------------------------------------
void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	float rangeValFloat = (float)rawValue;
	triggerValue = RangeMapFloat(rangeValFloat, 0.f, 256.f, 0.f, 1.f);
}

//------------------------------------------------------------------------------------------------------------------------------
void XboxController::UpdateJoystick( AnalogJoyStick& joystick, short rawX, short rawY )
{
	//normalize my shit
	float correctedRawX = RangeMapFloat(rawX, -32768.f, 32767.f, -1.f, 1.f);
	float correctedRawY = RangeMapFloat(rawY, -32768.f, 32767.f, -1.f, 1.f);
	joystick.UpdatePosition(correctedRawX, correctedRawY);
}

//------------------------------------------------------------------------------------------------------------------------------
void XboxController::UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short currentButtonFlag )
{
	bool isButtonSet = IsBitFlagSet( buttonFlags, currentButtonFlag);
	m_buttonStates[buttonID].UpdateStatus(isButtonSet);
}

//------------------------------------------------------------------------------------------------------------------------------
void XboxController::CheckControllerState()
{
	
}

//------------------------------------------------------------------------------------------------------------------------------
// Code not working
//------------------------------------------------------------------------------------------------------------------------------
void XboxController::SetupDebugController()
{
	TODO("Implement Debug Contoroller");

	//draw 3 circles

	//Debug mode quanitites
	Vec3 baseVectorOuter = Vec3(1.0f,0.f,0.f);
	Vec3 baseVectorCorrected = Vec3(0.9f,0.f,0.f);
	Vec3 baseVectorInner = Vec3(0.3f,0.f,0.f);
	Rgba deadZoneColor = Rgba(1.f, 1.f, 1.f, 1.f);
	Rgba correctedColor = Rgba(1.f, 1.f, 1.f, 1.f);
	int j = 0;
	for (float i = 0.f; i < 360; i+=22.5f)
	{
		//First vertex is always in the center
		m_outerDeadZone[j].m_position = Vec3(0.f, 0.f, 0.f);
		m_correctedZone[j].m_position = Vec3(0.f, 0.f, 0.f);
		m_innerDeadZone[j].m_position = Vec3(0.f, 0.f, 0.f);
		m_outerDeadZone[j].m_color = deadZoneColor;
		m_correctedZone[j].m_color = correctedColor;
		m_innerDeadZone[j].m_color = deadZoneColor;
		j++;

		m_outerDeadZone[j].m_position = baseVectorOuter.GetRotatedAboutZDegrees(i);
		m_outerDeadZone[j].m_position.SetLengthXY(m_outerDeadZoneRadius);
		m_correctedZone[j].m_position = baseVectorCorrected.GetRotatedAboutZDegrees(i);
		m_correctedZone[j].m_position.SetLengthXY(m_correctedMaxRadius);
		m_innerDeadZone[j].m_position = baseVectorInner.GetRotatedAboutZDegrees(i);
		m_innerDeadZone[j].m_position.SetLengthXY(m_innerDeadZoneRadius);
		m_outerDeadZone[j].m_color = deadZoneColor;
		m_correctedZone[j].m_color = correctedColor;
		m_innerDeadZone[j].m_color = deadZoneColor;
		j++;

		//Third Vertex
		m_outerDeadZone[j].m_position = baseVectorOuter.GetRotatedAboutZDegrees(i + 22.5f);
		m_outerDeadZone[j].m_position.SetLengthXY(m_outerDeadZoneRadius);
		m_correctedZone[j].m_position = baseVectorCorrected.GetRotatedAboutZDegrees(i + 22.5f);
		m_correctedZone[j].m_position.SetLengthXY(m_correctedMaxRadius);
		m_innerDeadZone[j].m_position = baseVectorInner.GetRotatedAboutZDegrees(i + 22.5f);
		m_innerDeadZone[j].m_position.SetLengthXY(m_innerDeadZoneRadius);
		m_outerDeadZone[j].m_color = deadZoneColor;
		m_correctedZone[j].m_color = correctedColor;
		m_innerDeadZone[j].m_color = deadZoneColor;
		j++;
	}

}

//------------------------------------------------------------------------------------------------------------------------------
// Broken too
//------------------------------------------------------------------------------------------------------------------------------
void XboxController::RenderDebugController() const
{
	TODO("Implement xbox Debugging");
	g_renderContext->DrawVertexArray(m_NUM_DEBUG_VERTICES, m_outerDeadZone);
	g_renderContext->DrawVertexArray(m_NUM_DEBUG_VERTICES, m_correctedZone);
	g_renderContext->DrawVertexArray(m_NUM_DEBUG_VERTICES, m_innerDeadZone);
}

