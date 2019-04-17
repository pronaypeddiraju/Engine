//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Commons/StringUtils.hpp"
#include "Engine/Math/Vec2.hpp"

#pragma warning( error: 4172) // Returning address of local variable or temporary variable
#pragma warning( error: 4172) //Not all control paths return a value

//------------------------------------------------------------------------------------------------------------------------------
//	DEBUG RENDER DEFAULTS
//------------------------------------------------------------------------------------------------------------------------------
// 2D Values
#define DEFAULT_DISC_THICKNESS	 (5.0f)
#define DEFAULT_LINE_WIDTH       (1.0f)
#define DEFAULT_POINT_SIZE       (10.0f)
#define DEFAULT_TEXT_HEIGHT		 (100.0f)
#define DEFAULT_WIRE_WIDTH_2D	 (1.f)

// 3D Values
#define DEFAULT_POINT_SIZE_3D    (1.0f)
#define DEFAULT_LINE_WIDTH_3D	 (0.1f)
#define DEFAULT_TEXT_HEIGHT_3D	 (0.1f)

// Log Values
#define DEFAULT_LOG_TEXT_HEIGHT  (15.f)

//------------------------------------------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);
#define KEY_UP 72
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define STATIC
#define BIT_FLAG(b)   (1 << (b))
#define ASSERT_RETURN_VALUE(assertThis, returnValue) if(!(assertThis)) {return returnValue;}
#define ASSERT(assertThis) if(!(assertThis)) {return false;}

//To do Info
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// the important bits
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
       " --------------------------------------------------------------------------------------\n" \
       "|  TODO :   " ##x "\n" \
       " --------------------------------------------------------------------------------------\n" )


#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" );// ASSERT_RECOVERABLE(0, "")


//------------------------------------------------------------------------------------------------------------------------------
class NamedStrings;
class EventSystems;
class PhysicsSystem;
class DevConsole;
class WindowContext;
class DebugRender;
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
extern DevConsole* g_devConsole;
extern EventSystems* g_eventSystem;
extern PhysicsSystem* g_physicsSystem;
extern NamedStrings g_gameConfigBlackboard; // declared in EngineCommon.hpp, defined in EngineCommon.cpp
extern WindowContext* g_windowContext;
extern DebugRender*	g_debugRenderer;
extern RenderContext* g_renderContext;

typedef NamedStrings EventArgs;

//------------------------------------------------------------------------------------------------------------------------------
const unsigned short KEY_ESC = 27;
const unsigned short UP_ARROW = 38;
const unsigned short DOWN_ARROW = 40;
const unsigned short LEFT_ARROW = 37;
const unsigned short RIGHT_ARROW = 39;
const unsigned short SPACE_KEY = 32;
const unsigned short DEL_KEY = 46;
const unsigned short BACK_SPACE = 8;
const unsigned short ENTER_KEY = 13;
const unsigned short KEY_LESSER = 188;
const unsigned short KEY_GREATER = 190;
const unsigned short M_KEY = 77;
const unsigned short N_KEY = 78;
const unsigned short T_KEY = 'T';
const unsigned short P_KEY = 'P';
const unsigned short W_KEY = 87;
const unsigned short A_KEY = 65;
const unsigned short B_KEY = 66;
const unsigned short S_KEY = 83;
const unsigned short D_KEY = 68;
const unsigned short G_KEY = 71;
const unsigned short K_KEY = 75;
const unsigned short L_KEY = 76;
const unsigned short Q_KEY = 81;
const unsigned short E_KEY = 69;
const unsigned short Z_KEY = 90;
const unsigned short X_KEY = 88;
const unsigned short C_KEY = 67;
const unsigned short V_KEY = 86;
const unsigned short F1_KEY = 112;
const unsigned short F2_KEY = 113;
const unsigned short F3_KEY = 114;
const unsigned short F4_KEY = 115;
const unsigned short F5_KEY = 116;
const unsigned short F6_KEY = 117;
const unsigned short F7_KEY = 118;
const unsigned short F8_KEY = 119;
const unsigned short NUM_1  = 49;
const unsigned short NUM_2  = 50;
const unsigned short NUM_3  = 51;
const unsigned short NUM_4  = 52;
const unsigned short NUM_5  = 53;
const unsigned short NUM_6  = 54;
const unsigned short NUM_7  = 55;
const unsigned short NUM_8  = 56;
const unsigned short NUM_9  = 57;
const unsigned short NUM_0  = 58;
const unsigned short TILDY_KEY = 192;
const unsigned short TAB_KEY = 9;
const unsigned short LCTRL_KEY = 17;

//------------------------------------------------------------------------------------------------------------------------------
constexpr char SHADER_PATH[] = "Data/Shaders/"; 
constexpr char IMAGE_PATH[] = "Data/Images/";
constexpr char FONT_PATH[] = "Data/Fonts/";
constexpr char MATERIAL_PATH[] = "Data/Materials/";

//------------------------------------------------------------------------------------------------------------------------------
constexpr float		 CONSOLE_LINE_SPACE = 0.05f;
constexpr int		 COLLIDER2D_COUNT = 6;

typedef unsigned int uint;
