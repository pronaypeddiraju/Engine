//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include <string>
#include <vector>

class BitmapFont;
class RenderContext;
struct Camera;

//------------------------------------------------------------------------------------------------------------------------------
struct ConsoleEntry
{
	const std::string		m_printString;
	Rgba					m_printColor;
	unsigned int	        m_frameNum;
	float					m_calledTime;

	explicit ConsoleEntry(const Rgba&, const std::string&, unsigned int, float);
};

//------------------------------------------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole();
	~DevConsole();
	void			Startup();
	void			BeginFrame();
	void			UpdateConsole(float deltaTime);
	void			EndFrame();
	void			Shutdown();

	void			SetBitmapFont(BitmapFont& bitmapFont);

	void			PrintString( const Rgba& textColor, const std::string& devConsolePrintString );
	void			Render( RenderContext& renderer, Camera& camera, float lineHeight ) const;

	void			ToggleOpenFull();
	bool			IsOpen() const;

	bool			ExecuteCommandLine(const std::string& commandLine);
	void			ShowLastCommand();
	void			ShowNextCommand();
	void			ResetIndexValues();

	inline int		GetFrameCount() {return m_frameCount;}

	// handle arrows, enter, escape, backspace, delete, etc...
	void			HandleKeyUp( unsigned char vkKeyCode );          // WM_KEYUP
	void			HandleKeyDown( unsigned char vkKeyCode );        // WM_KEYDOWN
	void			HandleCharacter( unsigned char charCode );       // WM_CHAR

public:
	//Public Event Systems test
	static bool		Command_Test(EventArgs& args);
	static bool		Command_Help(EventArgs& args);
	//Uses ExecuteCommandLine for now
	static bool		Command_Exec(EventArgs& args);


public:

 	const static Rgba						CONSOLE_INFO;
 	const static Rgba						CONSOLE_ERROR;
 	const static Rgba						CONSOLE_ERROR_DESC;
 	const static Rgba						CONSOLE_ECHO;
	const static Rgba						CONSOLE_BG_COLOR;
	const static Rgba						CONSOLE_INPUT;

private:
	BitmapFont*								m_consoleFont = nullptr;
	bool									m_isOpen = false;
	int										m_currentFrameNumber = 0;
	std::vector<ConsoleEntry>				m_printLog;
	std::vector<std::string>				m_commandLog;
	unsigned int							m_frameCount = 0;
	float									m_timeAtStart;

	std::string								m_currentInput;
	unsigned int							m_carotPosition = 0;

	float									m_carotTimeDiff = 0.f;
	bool									m_carotActive = true;

	unsigned int							m_lastCommandIndex = 0;
};