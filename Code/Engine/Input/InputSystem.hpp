//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Input/XboxController.hpp"

//------------------------------------------------------------------------------------------------------------------------------
constexpr unsigned int MAX_XBOX_CONTROLLERS = 4;

//------------------------------------------------------------------------------------------------------------------------------
//Key Codes
extern const unsigned short KEY_ESC;
extern const unsigned short UP_ARROW;
extern const unsigned short LEFT_ARROW;
extern const unsigned short RIGHT_ARROW;
extern const unsigned short SPACE_KEY;
extern const unsigned short A_KEY;
extern const unsigned short N_KEY;
extern const unsigned short T_KEY;
extern const unsigned short P_KEY;
extern const unsigned short F1_KEY;
extern const unsigned short F8_KEY;

//------------------------------------------------------------------------------------------------------------------------------
class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void						StartUp();
	void						BeginFrame();
	void						EndFrame();
	void						ShutDown();

	const XboxController&		GetXboxController (int controllerID);
	
	//This would be a good helper if you track controller assignment
	const XboxController&		GetNextAvailableXboxController();

private:
	XboxController m_controllers[MAX_XBOX_CONTROLLERS] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};
public:
	void						ResetController( int controllerID );
};
