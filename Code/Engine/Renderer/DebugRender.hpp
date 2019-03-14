//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Rgba.hpp"

struct AABB2;
struct AABB3;
struct Camera;
struct IntVec2;

class TextureView;
class RenderContext;

// System to render debugging data for specified objects in the system.
// This class works similar to any other Engine System. Requires a Startup call to initiate
// Requires a Draw call (Render) to render elements in the scene
// Requires a shutdown call to shut the system at the end.

//------------------------------------------------------------------------
#define DEFAULT_POINT_SIZE       (4.0f)
#define DEFAULT_LINE_WIDTH       (1.0f)
#define DEFAULT_HEIGHT			 ( 720.f )
#define DEFAULT_ASPECT			 ( 1.77f )

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


//------------------------------------------------------------------------
// struct to contain options for rendering something.  Contains parameters common to most 
// debug draw calls. 
struct DebugRenderOptionsT
{
	eDebugRenderSpace space       = DEBUG_RENDER_SCREEN; 
	eDebugRenderMode mode         = DEBUG_RENDER_USE_DEPTH; 

	float durationSeconds         = 0.0f;  // show for a single frame

	Rgba beginColor               = Rgba::WHITE; // color when this object first exist; 
	Rgba endColor                 = Rgba::WHITE; // color when this object is about to be destroyed

	// 2D common - these are where I will treat the "origin" 
	// of the screen for drawing - defaulting to bottom-left corner; 
	// used for SCREEN and CAMERA modes, ignored for WORLD
	Vec4 positionRatioAndOffset   = Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); // this is bottom-left corner 

	// 3D (WORLD space) common (ignored for SCREEN & CAMERA)
	Matrix44 modelTransform       = Matrix44::IDENTITY; // local transform to do to the object; 
};

class DebugRender
{
public:
	DebugRender();
	~DebugRender();

	void					Startup(RenderContext* renderContext);
	void					Shutdown();
	void					BeginFrame();
	void					Update(float deltaTime);
	void					Render() const;

	Camera&					Get2DCamera();

	//------------------------------------------------------------------------
	// 2D Debug Rendering (defaults to SCREEN)
	//------------------------------------------------------------------------
	void				DebugRenderPoint2D( DebugRenderOptionsT const &options, 
											 Vec2 position, 
											 float size = DEFAULT_POINT_SIZE );

	void				DebugRenderLine2D( DebugRenderOptionsT const &options, 
											Vec2 start, Vec2 end, 
											float lineWidth = DEFAULT_LINE_WIDTH );

	void				DebugRenderQuad2D( DebugRenderOptionsT const &options, 
											AABB2 const &quad ); 

	void				DebugRenderTexturedQuad2D( DebugRenderOptionsT const &options, 
													AABB2 const &quad, 
													TextureView *view ); 

	void				DebugRenderWireQuad2D( DebugRenderOptionsT const &options, 
												AABB2 const &quad, 
												float lineWidth = DEFAULT_LINE_WIDTH ); 

	// to get a ring, you can jus tuse a innerRadius line-thickness smaller than radius; 
	void				DebugRenderDisc2D( DebugRenderOptionsT const &options, 
											Vec2 center, 
											float radius, 
											float innerRadius = 0.0f ); 
	
	//------------------------------------------------------------------------
	// 3D Rendering (will always default to WORLD)
	//------------------------------------------------------------------------
	void				DebugRenderPoint( DebugRenderOptionsT const &options, 
											Vec3 position, 
											float size = DEFAULT_POINT_SIZE );

	void				DebugRenderPoint( Camera& camera, DebugRenderOptionsT const &options,
											Vec3 position,
											TextureView* texture = nullptr,
											float size = DEFAULT_POINT_SIZE );

	void				DebugRenderLine( DebugRenderOptionsT const &options, 
											Vec3 start, Vec3 end, 
											float lineWidth = DEFAULT_LINE_WIDTH );

	void				DebugRenderArrow3D( DebugRenderOptionsT const &options, 
											Vec3 start, Vec3 end, 
											float base_thickness, 
											float head_thickness ); 

	void				DebugRenderSphere( DebugRenderOptionsT const &options, 
										   Vec3 center, 
										   float radius ); 

	void				DebugRenderBox( DebugRenderOptionsT const &options, 
										AABB3 box ); 

	// EXTRA (helps to be able to set raster fill mode to "wire")
	// Also, better to use an ICOSphere if available, but UV sphere is fine; 
	void				DebugRenderWireSphere( DebugRenderOptionsT const &options,
											   Vec3 center, 
											   float radius ); 

	// EXTRA (helps to be able to set raster fill mode to "wire")
	void				DebugRenderWireBox( DebugRenderOptionsT const &options, 
											AABB3 box ); 

	// EXTRA (requires being able to render a cone/cylinder)
	void				DebugRenderArrow( DebugRenderOptionsT const &options, 
										  Vec3 start, Vec3 end, 
										  float lineWidth = DEFAULT_LINE_WIDTH ); 

	// EXTRA - requires Arrow
	void				DebugRenderBasis( DebugRenderOptionsT const &options, 
										  Matrix44 const &basis, 
										  float lineWidth = DEFAULT_LINE_WIDTH ); 

private:
	IntVec2				ConvertWorldToScreenPoint(Vec3 worldPoint);
	Vec3				ConvertScreenToWorldPoint(Vec3 screenPoint);

private:
	RenderContext*		m_renderContext = nullptr;
	int					m_clientWidth;
	int					m_clientHeight;
	Camera*				m_debug3DCam = nullptr;
	Camera*				m_debug2DCam = nullptr;
	bool				m_canRender = true;
};

/*
//------------------------------------------------------------------------
// Rendering Text (works in any mode)
//------------------------------------------------------------------------
void DebugRenderTextv( DebugRenderOptionsT const &options, 
	Vec2 pivot, char const *format, va_list args );
void DebugRenderTextf( DebugRenderOptionsT const &options, 
	Vec2 pivot, char const *format, ... ); 

//------------------------------------------------------------------------
// Logs (shows up on screen temporily in a list)
// ALWAYS assumes screen space; 
//------------------------------------------------------------------------
void DebugRenderLogv( DebugRenderOptionsT const &options, 
	char const *format, va_list args ); 
void DebugRenderLogf( DebugRenderOptionsT const &options, 
	char const *format); 
*/
