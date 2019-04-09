//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <algorithm>

DevConsole* g_devConsole = nullptr;

const STATIC Rgba DevConsole::CONSOLE_INFO			=	Rgba(1.0f, 1.0f, 0.0f, 1.0f);
const STATIC Rgba DevConsole::CONSOLE_BG_COLOR		=	Rgba(0.0f, 0.0f, 0.0f, 0.75f);
const STATIC Rgba DevConsole::CONSOLE_ERROR   		=	Rgba(1.0f, 0.0f, 0.0f, 1.0f);
const STATIC Rgba DevConsole::CONSOLE_ERROR_DESC	=	Rgba(1.0f, 0.5f, 0.3f, 1.0f);
const STATIC Rgba DevConsole::CONSOLE_INPUT			=	Rgba(1.0f, 1.0f, 1.0f, 1.0f);

//------------------------------------------------------------------------------------------------------------------------------
bool DevConsole::ExecuteCommandLine( const std::string& commandLine )
{
	//Split the string to sensible key value pairs
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commandLine, ' ');
	if(splitStrings[0] != "Exec")
	{
		return false;
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

		return true;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::HandleKeyUp( unsigned char vkKeyCode )
{
	UNUSED(vkKeyCode);
	GUARANTEE_RECOVERABLE(true, "Nothing to handle");
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::ShowLastCommand()
{
	if(m_lastCommandIndex == 0)
	{
		int cmdSize = static_cast<int>(m_commandLog.size());
		if(cmdSize > 0)
		{
			m_lastCommandIndex = static_cast<int>(m_commandLog.size()) - 1;
			m_currentInput = m_commandLog[m_lastCommandIndex];
		}
	}
	else
	{
		if(m_lastCommandIndex > 0)
		{
			m_lastCommandIndex--;
			m_currentInput = m_commandLog[m_lastCommandIndex];
		}
	}

	m_carotPosition = static_cast<int>(m_currentInput.size());
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::ShowNextCommand()
{
	if(m_lastCommandIndex == static_cast<unsigned int>(m_commandLog.size()) - 1)
	{
		m_lastCommandIndex = 0;
		m_currentInput = m_commandLog[m_lastCommandIndex];
	}
	else
	{
		m_lastCommandIndex++;
		m_currentInput = m_commandLog[m_lastCommandIndex];
	}

	m_carotPosition = static_cast<int>(m_currentInput.size());
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::HandleKeyDown( unsigned char vkKeyCode )
{
	switch( vkKeyCode )
	{
	case UP_ARROW:
	{
		//Show the last command executed
		ShowLastCommand();
	}
	break;
	case DOWN_ARROW:
	{
		ShowNextCommand();
	}
	break;
	case LEFT_ARROW:
	{
		if(m_carotPosition > 0)
		{
			m_carotPosition -= 1;
		}
	}
	break;
	case RIGHT_ARROW:
	{
		if(m_carotPosition < static_cast<int>(m_currentInput.size()))
		{
			m_carotPosition += 1;
		}
	}
	break;
	case DEL_KEY:
	{
		//Delete the char after carot
		if(m_carotPosition < static_cast<int>(m_currentInput.size()))
		{
			std::string firstSubString = m_currentInput.substr(0, m_carotPosition);
			std::string secondSubString = m_currentInput.substr(m_carotPosition + 1, m_currentInput.size());

			m_currentInput = firstSubString;
			m_currentInput += secondSubString;
		}

		ResetIndexValues();
	}
	break;
	case BACK_SPACE:
	{
		if(m_carotPosition > 0)
		{
			std::string firstSubString = m_currentInput.substr(0, m_carotPosition - 1);
			std::string secondSubString = m_currentInput.substr(m_carotPosition, m_currentInput.size());

			m_currentInput = firstSubString;
			m_currentInput += secondSubString;

			m_carotPosition--;
		}

		ResetIndexValues();
	}
	break;
	case KEY_ESC:
	{
		if(m_currentInput.size() > 0)
		{
			//Clear the input string
			m_currentInput.clear();
			m_carotPosition = 0;
		}
		else
		{
			//shut the console
			m_isOpen = false;
		}
	}
	break;
	case ENTER_KEY:
	{
		//Run inputString as command only if not empty
		if(m_currentInput.size() == 0)
		{
			return;
		}

		bool result = g_eventSystem->FireEvent(m_currentInput);

		if(!result)
		{
			PrintString(CONSOLE_ERROR, m_currentInput + ": Command was not found");
		}
		
		//Store the last command that was executed but remove copies

		// order of unique elements is preserved
		auto end_unique = std::end(m_commandLog) ;
		for( auto iter = std::begin(m_commandLog) ; iter != end_unique ; ++iter )
		{
			end_unique = std::remove( iter+1, end_unique, *iter ) ;
		}
		m_commandLog.erase( end_unique, std::end(m_commandLog) ) ;

		
		auto findItr = std::find(m_commandLog.begin(), m_commandLog.end(), m_currentInput);
		if(findItr != m_commandLog.end())
		{
			m_commandLog.erase(findItr);
		}

		m_commandLog.push_back(m_currentInput);

		m_currentInput.clear();
		m_carotPosition = 0;
		m_lastCommandIndex = 0;
	}
	break;
	default:
	break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::ResetIndexValues()
{
	m_lastCommandIndex = 0;
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::HandleCharacter( unsigned char charCode )
{
	if(m_carotPosition == m_currentInput.size())
	{
		m_currentInput += charCode;
		m_carotPosition += 1;
	}
	else
	{
		//carot is not at the end so add at the correct pos
		std::string m_firstSubString = m_currentInput.substr(0, m_carotPosition);
		std::string m_secondSubString = m_currentInput.substr(m_carotPosition, m_currentInput.size());

		m_currentInput = m_firstSubString;
		m_currentInput += charCode;
		m_currentInput += m_secondSubString;

		m_carotPosition++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
const STATIC Rgba DevConsole::CONSOLE_ECHO		=	Rgba(0.255f, 0.450f, 1.0f, 1.0f);

//------------------------------------------------------------------------------------------------------------------------------
DevConsole::DevConsole()
{
	m_timeAtStart = static_cast<float>(GetCurrentTimeSeconds());
}

//------------------------------------------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	g_eventSystem->SubscribeEventCallBackFn( "Test", Command_Test );
	g_eventSystem->SubscribeEventCallBackFn( "Help", Command_Help );
	m_currentInput.empty();
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::UpdateConsole(float deltaTime)
{
	m_carotTimeDiff += deltaTime;

	if(m_carotTimeDiff > 0.5f)
	{
		m_carotActive = !m_carotActive;
		m_carotTimeDiff = 0.f;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
	m_frameCount++;
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::SetBitmapFont( BitmapFont& bitmapFont )
{
	m_consoleFont = &bitmapFont;
}

//------------------------------------------------------------------------------------------------------------------------------
ConsoleEntry::ConsoleEntry(const Rgba& textColor, const std::string& printString, unsigned int frameNum, float frameTime)
	: m_printColor (textColor),
	  m_printString (printString),
	  m_frameNum (frameNum),
	  m_calledTime (frameTime)
{

}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::PrintString( const Rgba& textColor, const std::string& devConsolePrintString )
{
	float time = static_cast<float>(GetCurrentTimeSeconds());
	time -= m_timeAtStart;
	ConsoleEntry consoleEntry = ConsoleEntry(textColor, devConsolePrintString, m_frameCount, time);
	m_printLog.push_back(consoleEntry);
}

//------------------------------------------------------------------------------------------------------------------------------
void DevConsole::Render( RenderContext& renderer, Camera& camera, float lineHeight ) const
{
	camera.SetModelMatrix(Matrix44::IDENTITY);
	camera.UpdateUniformBuffer(&renderer);

	renderer.BeginCamera(camera);

	renderer.BindTextureViewWithSampler( 0U, m_consoleFont->GetTexture()); 

	//Draw a black box over the entire screen
	AABB2 blackBox = AABB2(camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight());
	std::vector<Vertex_PCU> boxVerts;
	AddVertsForAABB2D(boxVerts, blackBox, CONSOLE_BG_COLOR);

	//Set the text based on Camera size
	Vec2 leftBot = camera.GetOrthoBottomLeft();
	Vec2 rightTop = camera.GetOrthoTopRight();

	float screenHeight = rightTop.y - leftBot.y;
	float screenWidth = rightTop.x - leftBot.x;

	int numLines = static_cast<int>(screenHeight / lineHeight);
	int numStrings = static_cast<int>(m_printLog.size());

	Vec2 boxBottomLeft = leftBot + Vec2(lineHeight, lineHeight * 2);
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

	//Draw Text Input box
	Vec2 textBoxBottomLeft = leftBot + Vec2(lineHeight, lineHeight * 0.25f);
	Vec2 textBoxTopRight = textBoxBottomLeft + Vec2(screenWidth - lineHeight * 2, lineHeight * 1.25f);

	AABB2 inputBox = AABB2(textBoxBottomLeft, textBoxTopRight);

	renderer.BindTextureView(0U, nullptr);
	
	std::vector<Vertex_PCU> inputBoxVerts;
	AddVertsForBoundingBox(inputBoxVerts, inputBox, CONSOLE_INFO, lineHeight * 0.1f);
	renderer.DrawVertexArray(inputBoxVerts);

	//Draw current string
	renderer.BindTextureView(0U, m_consoleFont->GetTexture());

	std::vector<Vertex_PCU> inputStringVerts;
	m_consoleFont->AddVertsForTextInBox2D(inputStringVerts, inputBox, lineHeight, m_currentInput, CONSOLE_INPUT, 1.f, Vec2::ALIGN_LEFT_CENTERED);
	renderer.DrawVertexArray(inputStringVerts);

	if(m_carotActive)
	{
		//Draw carot
		renderer.BindTextureView(0U, nullptr);

		Vec2 carotStart = inputBox.m_minBounds;
		carotStart.x += m_carotPosition * lineHeight;
		Vec2 carotEnd = carotStart + Vec2(0.f, lineHeight * 1.25f);

		std::vector<Vertex_PCU> carotVerts;
		AddVertsForLine2D(carotVerts, carotStart, carotEnd, lineHeight * 0.1f, CONSOLE_INPUT);
		renderer.DrawVertexArray(carotVerts);
	}

	renderer.EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
bool DevConsole::IsOpen() const
{
	return m_isOpen;
}

//------------------------------------------------------------------------------------------------------------------------------
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
