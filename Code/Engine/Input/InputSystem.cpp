//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"

InputSystem* g_inputSystem = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
InputSystem::InputSystem()
{
}

//------------------------------------------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{
}

//------------------------------------------------------------------------------------------------------------------------------
void InputSystem::StartUp()
{
}

//------------------------------------------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	//Check controller status here 
	for(int i = 0; i < MAX_XBOX_CONTROLLERS; i++)
	{
		m_controllers[i].Update();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void InputSystem::ShutDown()
{

}

//------------------------------------------------------------------------------------------------------------------------------
const XboxController& InputSystem::GetXboxController( int controllerID )	
{
	return m_controllers[controllerID];
}

//------------------------------------------------------------------------------------------------------------------------------
int InputSystem::GetNumConnectedControllers()
{
	int numConnected = 0;

	//Check how many controllers have been connected in the system
	for (int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; controllerIndex++)
	{
		if (m_controllers[controllerIndex].IsConnected())
		{
			numConnected++;
		}
	}

	return numConnected;
}

//------------------------------------------------------------------------------------------------------------------------------
void InputSystem::ResetController( int controllerID )
{
	m_controllers[controllerID].Reset();
}
