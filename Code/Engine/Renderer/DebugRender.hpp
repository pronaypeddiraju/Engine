//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/DebugObjectProperties.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <vector>

struct AABB2;
struct AABB3;
struct Camera;
struct IntVec2;
class Disc2D;

class TextureView;
class RenderContext;

// System to render debugging data for specified objects in the system.
//------------------------------------------------------------------------
enum eDebugRenderSpace 
{
	DEBUG_RENDER_SCREEN,          // renders in screen space (ie, backbuffer), defined when initializing the system 
	DEBUG_RENDER_WORLD,           // is rendered in the world; 
}; 

enum eDebugRenderMode
{
	DEBUG_RENDER_USE_DEPTH,    
	DEBUG_RENDER_ALWAYS,       // always draw, does not write to depth
	DEBUG_RENDER_XRAY,         // render behind (greater, no write), then in front (lequal, write)
							   // darken or thin the line during behind render to differentiate from it being in front; 
}; 

//------------------------------------------------------------------------
struct DebugRenderOptionsT
{
	eDebugRenderSpace space       = DEBUG_RENDER_SCREEN; 
	eDebugRenderMode mode         = DEBUG_RENDER_USE_DEPTH; 

	Rgba beginColor               = Rgba::WHITE; 
	Rgba endColor                 = Rgba::WHITE; 

	// 2D common - these are where I will treat the "origin" 
	// of the screen for drawing - defaulting to bottom-left corner; 
	// used for SCREEN and CAMERA modes, ignored for WORLD
	Vec4 positionRatioAndOffset   = Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); // this is bottom-left corner 

	// 3D (WORLD space) common (ignored for SCREEN & CAMERA)
	Matrix44 modelTransform       = Matrix44::IDENTITY; // local transform to do to the object; 
	
	ObjectProperties* objectProperties = nullptr; 
};

class DebugRender
{
public:
	DebugRender();
	~DebugRender();

	void					Startup(RenderContext* renderContext);
	void					SetClientDimensions(int height, int width);
	void					Shutdown();
	void					BeginFrame();
	void					EndFrame();
	void					Update(float deltaTime);
	void					CleanUpObjects();

	//void					RenderDebugObjects(RenderContext* renderContext) const;

	void					DebugRenderToScreen() const;		//This renders to the screen camera
	void					DebugRenderToCamera() const;		//This renders in the world camera

	void					Setup2DCamera() const;
	void					Setup3DCamera(Camera* const camera);
	void					SetObjectMatrixForPosition( Vec3 position ) const;

	Camera&					Get2DCamera();

	//------------------------------------------------------------------------
	// 2D Debug Rendering (defaults to SCREEN)
	//------------------------------------------------------------------------
	void				DebugRenderPoint2D( DebugRenderOptionsT options, const Vec2& position, float duration = 0.f, float size = DEFAULT_POINT_SIZE );
	void				DebugRenderLine2D( DebugRenderOptionsT options, const Vec2& start, const Vec2& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH );
	void				DebugRenderQuad2D( DebugRenderOptionsT options, AABB2 const &quad, float duration = 0.f, TextureView *view = nullptr); 
	void				DebugRenderWireQuad2D( DebugRenderOptionsT options, AABB2 const &quad, float duration = 0.f, float thickness = DEFAULT_WIRE_WIDTH_2D ); 
	void				DebugRenderDisc2D( DebugRenderOptionsT options, Disc2D const &disc, float duration = 0.f); 
	void				DebugRenderRing2D( DebugRenderOptionsT options, Disc2D const &disc, float duration = 0.f, float thickness = DEFAULT_DISC_THICKNESS ); 

	//------------------------------------------------------------------------
	// 3D Rendering (will always default to WORLD)
	//------------------------------------------------------------------------
	void				DebugRenderPoint( DebugRenderOptionsT options, const Vec3& position, float duration = 0.f, float size = DEFAULT_POINT_SIZE_3D, TextureView* texture = nullptr );
	void				DebugRenderLine( DebugRenderOptionsT options, const Vec3& start, const Vec3& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH );
	void				DebugRenderSphere( DebugRenderOptionsT options, Vec3 center, float radius, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderBox( DebugRenderOptionsT options, const AABB3& box, float duration = 0.f, TextureView* texture = nullptr ); 
	
	void				DebugRenderArrow3D( DebugRenderOptionsT options, Vec3 start, Vec3 end, float base_thickness, float head_thickness ); 

	// EXTRA (helps to be able to set raster fill mode to "wire")
	// Also, better to use an ICOSphere if available, but UV sphere is fine; 
	void				DebugRenderWireSphere( DebugRenderOptionsT options, Vec3 center, float radius ); 

	// EXTRA (helps to be able to set raster fill mode to "wire")
	void				DebugRenderWireBox( DebugRenderOptionsT options, AABB3 box ); 

	// EXTRA (requires being able to render a cone/cylinder)
	void				DebugRenderArrow( DebugRenderOptionsT options, Vec3 start, Vec3 end, float lineWidth = DEFAULT_LINE_WIDTH ); 

	// EXTRA - requires Arrow
	void				DebugRenderBasis( DebugRenderOptionsT const &options, Matrix44 const &basis, float lineWidth = DEFAULT_LINE_WIDTH ); 

private:
	IntVec2							ConvertWorldToScreenPoint(Vec3 worldPoint);
	Vec3							ConvertScreenToWorldPoint(Vec3 screenPoint);

	//Draw methods 2D
	void							DrawPoint2D( const DebugRenderOptionsT* renderObject ) const;
	void							DrawLine2D( const DebugRenderOptionsT* renderObject ) const;
	void							DrawQuad2D( const DebugRenderOptionsT* renderObject ) const;
	void							DrawWireQuad2D(const DebugRenderOptionsT* renderObject ) const;
	void							DrawDisc2D( const DebugRenderOptionsT* renderObject ) const;
	void							DrawRing2D( const DebugRenderOptionsT* renderObject ) const;

	//Draw methods 3D
	void							DrawPoint3D( const DebugRenderOptionsT* renderObject ) const;
	void							DrawLine3D( const DebugRenderOptionsT* renderObject ) const;
	void							DrawSphere( const DebugRenderOptionsT* renderObject ) const;
	void							DrawBox( const DebugRenderOptionsT* renderObject ) const;

private:
	RenderContext*		m_renderContext					= nullptr;
	int					m_clientWidth					= 0;
	int					m_clientHeight					= 0;
	Camera*				m_debug3DCam					= nullptr;
	Camera*				m_debug2DCam					= nullptr;
	bool				m_canRender						= true;

	//Store all debug objects with their render options and other data
	std::vector<DebugRenderOptionsT>	worldRenderObjects;
	std::vector<DebugRenderOptionsT>	screenRenderObjects;
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
