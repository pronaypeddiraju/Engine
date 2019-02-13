#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Rgba.hpp"

DevConsole* g_devConsole = nullptr;

const STATIC Rgba DevConsole::CONSOLE_INFO			=	Rgba(1.0f, 1.0f, 0.0f, 1.0f);
const STATIC Rgba DevConsole::CONSOLE_BG_COLOR		=	Rgba(0.0f, 0.0f, 0.0f, 0.75f);
const STATIC Rgba DevConsole::CONSOLE_ERROR   		=	Rgba(1.0f, 0.0f, 0.0f, 1.0f);
const STATIC Rgba DevConsole::CONSOLE_ERROR_DESC	=	Rgba(1.0f, 0.5f, 0.3f, 1.0f);

void DevConsole::ExecuteCommandLine( const std::string& commandLine )
{
	//Split the string to sensible key value pairs
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commandLine, ' ');
	if(splitStrings[0] != "Exec")
	{
		return;
	}

	else
	{
		g_devConsole->PrintString(CONSOLE_INFO, "Data Received:");
		std::string printS = "> Exec ";
		for(int i =0; i < static_cast<int>(splitStrings.size()); i++)
		{
			printS += " " + splitStrings[i];
		}
		g_devConsole->PrintString(CONSOLE_INFO, printS);


		for(int stringIndex = 1; stringIndex < static_cast<int>(splitStrings.size()); stringIndex++)
		{
			//split on =
			std::vector<std::string> KeyValSplit = SplitStringOnDelimiter(splitStrings[stringIndex], '=');
			if(KeyValSplit.size() != 2)
			{
				g_devConsole->PrintString(CONSOLE_ERROR ," ! The number of arguments read are not valid");
				g_devConsole->PrintString(CONSOLE_ERROR_DESC, "    Execute requires 2 arguments. A key and value pair split by =");
			}
			else
			{
				//Print the data we read
				printS = " Action: " + KeyValSplit[0] + " = " + KeyValSplit[1];
				g_devConsole->PrintString(CONSOLE_ECHO, printS);
			}
		}
	}
}

const STATIC Rgba DevConsole::CONSOLE_ECHO		=	Rgba(0.255f, 0.450f, 1.0f, 1.0f);

DevConsole::DevConsole()
{
	m_timeAtStart = static_cast<float>(GetCurrentTimeSeconds());
}

DevConsole::~DevConsole()
{

}

void DevConsole::Startup()
{
	g_eventSystem->SubscribeEventCallBackFn( "Test", Command_Test );
	g_eventSystem->SubscribeEventCallBackFn( "Help", Command_Help );
}

void DevConsole::BeginFrame()
{
	
}

void DevConsole::EndFrame()
{
	m_frameCount++;
}

void DevConsole::Shutdown()
{

}

void DevConsole::SetBitmapFont( BitmapFont& bitmapFont )
{
	m_consoleFont = &bitmapFont;
}

ConsoleEntry::ConsoleEntry(const Rgba& textColor, const std::string& printString, unsigned int frameNum, float frameTime)
	: m_printColor (textColor),
	  m_printString (printString),
	  m_frameNum (frameNum),
	  m_calledTime (frameTime)
{

}

void DevConsole::PrintString( const Rgba& textColor, const std::string& devConsolePrintString )
{
	float time = static_cast<float>(GetCurrentTimeSeconds());
	time -= m_timeAtStart;
	ConsoleEntry consoleEntry = ConsoleEntry(textColor, devConsolePrintString, m_frameCount, time);
	m_printLog.push_back(consoleEntry);
}

void DevConsole::Render( RenderContext& renderer, const Camera& camera, float lineHeight ) const
{
	//Draw a black box over the entire screen
	AABB2 blackBox = AABB2(camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight());
	std::vector<Vertex_PCU> boxVerts;
	AddVertsForAABB2D(boxVerts, blackBox, CONSOLE_BG_COLOR);

	//DEBUG
	//renderer.BindTextureView(0U, nullptr);
	//renderer.DrawVertexArray(boxVerts);

	//Set the text based on Camera size
	Vec2 leftBot = camera.GetOrthoBottomLeft();
	Vec2 rightTop = camera.GetOrthoTopRight();

	float screenHeight = rightTop.y - leftBot.y;
	int numLines = static_cast<int>(screenHeight / lineHeight);
	int numStrings = static_cast<int>(m_printLog.size());

	Vec2 boxBottomLeft = leftBot + Vec2(lineHeight, lineHeight);
	Vec2 boxTopRight = boxBottomLeft;

	//Get the last string in the map and work your way back
	std::vector<ConsoleEntry>::const_iterator vecIterator = m_printLog.end();

	std::vector<Vertex_PCU> textVerts;
	renderer.BindTextureView(0U, m_consoleFont->GetTexture());

	//Setup your loop end condition
	int endCondition = 0;
	if(numLines < numStrings)
	{
		endCondition = numLines;
	}
	else
	{
		endCondition = numStrings;
	}

	for(int lineIndex = 0; lineIndex < endCondition; lineIndex++)
	{
		//Decrement the iterator
		vecIterator--;

		textVerts.empty();

		//Get length of string
		std::string printString = "[ T:" + Stringf("%03f", vecIterator->m_calledTime) + " Frame:" + std::to_string(vecIterator->m_frameNum) + " ] " + vecIterator->m_printString;
		int numChars = static_cast<int>(printString.length());

		//Create the required box
		float glyphWidth = lineHeight * m_consoleFont->GetGlyphAspect(0);
		boxTopRight = boxBottomLeft;
		boxTopRight += Vec2(numChars * glyphWidth , lineHeight);
		AABB2 printBox = AABB2(boxBottomLeft, boxTopRight);

		//Print the text
		m_consoleFont->AddVertsForTextInBox2D(textVerts, printBox, lineHeight, printString, vecIterator->m_printColor, m_consoleFont->GetGlyphAspect(0), Vec2::ALIGN_LEFT_CENTERED);
		renderer.DrawVertexArray(textVerts);

		//Change the boxBottomLeft based on line height
		boxBottomLeft.y += lineHeight;

		//Add a padding if desired
		//boxBottomLeft.y += CONSOLE_LINE_SPACE;
	}

	renderer.BindTextureView(0U, nullptr);
}

void DevConsole::ToggleOpenFull()
{
	if(m_isOpen)
	{
		m_isOpen = false;
	}
	else
	{
		m_isOpen = true;
	}
}

bool DevConsole::IsOpen() const
{
	return m_isOpen;
}

//------------------------------------------------------------------------------------------------
STATIC bool DevConsole::Command_Test( EventArgs& args )
{
	UNUSED( args );
	g_devConsole->PrintString( CONSOLE_INFO, Stringf( "Test command received" ) );
	//args.DebugPrintContents();
	return true; // Does not consume event; continue to call other subscribers’ callback functions
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool	DevConsole::Command_Help( EventArgs& args )
{
	UNUSED(args);
	g_devConsole->PrintString( CONSOLE_INFO, Stringf( "> Help Commands available to user" ) );
	std::vector<std::string> eventsAvailable;
	g_eventSystem->GetSubscribedEventsList(eventsAvailable);
	for(int eventIndex = 0; eventIndex < static_cast<int>(eventsAvailable.size()); eventIndex++)
	{
		std::string printString =  "   " + eventsAvailable[eventIndex];
		g_devConsole->PrintString( CONSOLE_ECHO, printString );
	}
	return true;
}
