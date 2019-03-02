//------------------------------------------------------------------------------------------------------------------------------
#pragma once

// System to render debugging data for specified objects in the system.
// This class works similar to any other Engine System. Requires a Startup call to initiate
// Requires a Draw call (Render) to render elements in the scene
// Requires a shutdown call to shut the system at the end.

//------------------------------------------------------------------------
#define DEFAULT_POINT_SIZE       (4.0f)
#define DEFAULT_LINE_WIDTH       (1.0f)

//------------------------------------------------------------------------
enum eDebugRenderSpace 
{
	DEBUG_RENDER_SCREEN,          // renders in screen space (ie, backbuffer), defined when initializing the system 
	DEBUG_RENDER_WORLD,           // is rendered in the world; 
}; 

enum eDebugRenderMode
{
	DEBUG_RENDER_USE_DEPTH,    // default mode - will use depth buffer and write;
	DEBUG_RENDER_ALWAYS,       // always draw, does not write to depth
	DEBUG_RENDER_XRAY,         // render behind (greater, no write), then in front (lequal, write)
								// darken or thin the line during behind render to differentiate from it being in front; 
}; 
