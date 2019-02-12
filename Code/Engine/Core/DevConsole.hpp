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
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void SetBitmapFont(BitmapFont& bitmapFont);

	void PrintString( const Rgba& textColor, const std::string& devConsolePrintString );
	void Render( RenderContext& renderer, const Camera& camera, float lineHeight ) const;

	void ToggleOpenFull();
	bool IsOpen() const;

	void ExecuteCommandLine(const std::string& commandLine);

public:
	//Public Event Systems test
	static bool Command_Test(EventArgs& args);
	static bool	Command_Help(EventArgs& args);
	//Uses ExecuteCommandLine for now
	static bool	Command_Exec(EventArgs& args);


public:

 	const static Rgba CONSOLE_INFO;
 	const static Rgba CONSOLE_ERROR;
 	const static Rgba CONSOLE_ERROR_DESC;
 	const static Rgba CONSOLE_ECHO;
	const static Rgba CONSOLE_BG_COLOR;

private:
	BitmapFont*								m_consoleFont = nullptr;
	bool									m_isOpen = false;
	int										m_currentFrameNumber = 0;
	std::vector<ConsoleEntry>				m_printLog;
	unsigned int							m_frameCount = 0;
	float									m_timeAtStart;
};