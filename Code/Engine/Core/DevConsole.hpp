//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Commons/LogSystem.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <string>
#include <vector>
#include "Engine/Core/Async/AsyncQueue.hpp"
#include <shared_mutex>

class BitmapFont;
class RenderContext;
struct AABB2;
struct Camera;
struct Vertex_PCU;

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
	void			GetVertsForDevConsoleMemTracker(std::vector<Vertex_PCU>& textVerts, AABB2& memTrackingBox, float lineHeight) const;

	void			PrintString( const Rgba& textColor, const std::string& devConsolePrintString );
	void			Render( RenderContext& renderer, Camera& camera, float lineHeight ) const;
	void			RenderMemTrackingInfo(RenderContext& renderer, Camera& camera, float lineHeight) const;

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
	static bool		Command_Clear(EventArgs& args);
	static bool		Command_MemTracking(EventArgs& args);
	static bool		Command_MemLog(EventArgs& args);

	static bool		Command_EnableAllLogFilters(EventArgs& args);
	static bool		Command_DisableAllLogfilters(EventArgs& args);
	static bool		Command_EnableLogFilter(EventArgs& args);
	static bool		Command_DisableLogFilter(EventArgs& args);
	static bool		Command_FlushLogSystem(EventArgs& args);
	static bool		Command_Logf(EventArgs& args);
	//Uses ExecuteCommandLine for now
	static bool		Command_Exec(EventArgs& args);

public:

 	const static Rgba						CONSOLE_INFO;
 	const static Rgba						CONSOLE_ERROR;
 	const static Rgba						CONSOLE_ERROR_DESC;
 	const static Rgba						CONSOLE_ECHO_COLOR;
	const static Rgba						CONSOLE_BG_COLOR;
	const static Rgba						CONSOLE_INPUT;

	std::shared_mutex						m_mutexLock;

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

	bool									m_memTrackingEnabled = true;
	Vec2									m_memTrackingBoxSize = Vec2(80.f,10.f);
	
};

void			LogHookForDevConsole(const LogObject_T* logObj);