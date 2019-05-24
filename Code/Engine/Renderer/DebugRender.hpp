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
struct Capsule3D;
struct IntVec2;

class BitmapFont;
class Disc2D;
class TextureView;
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
// System to render debugging data for specified objects in the system.
//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
struct DebugRenderOptionsT
{
	eDebugRenderSpace space       = DEBUG_RENDER_SCREEN; 
	eDebugRenderMode mode         = DEBUG_RENDER_USE_DEPTH; 

	Rgba beginColor               = Rgba::WHITE; 
	Rgba endColor                 = Rgba::RED; 

	// 2D common - these are where I will treat the "origin" 
	// of the screen for drawing - defaulting to bottom-left corner; 
	// used for SCREEN and CAMERA modes, ignored for WORLD
	Vec4 positionRatioAndOffset   = Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); // this is bottom-left corner 

	// 3D (WORLD space) common (ignored for SCREEN & CAMERA)
	Matrix44 modelTransform       = Matrix44::IDENTITY; // local transform to do to the object; 
	
	bool relativeCoordinates = false;

	ObjectProperties* objectProperties = nullptr; 
};

//------------------------------------------------------------------------------------------------------------------------------
class DebugRender
{
public:
	DebugRender();
	~DebugRender();

	//Events for Event system and dev console
	static bool				DisableDebugRender( EventArgs& args );
	static bool				EnableDebugRender( EventArgs& args );
	static bool				ClearAllLiveObjects( EventArgs& args );
	static bool				ClearAllLiveScreenObjects( EventArgs& args );
	static bool				ClearAllLiveWorldObjects( EventArgs& args );

	void					Startup(RenderContext* renderContext);
	void					SetClientDimensions(int height, int width);
	void					SetDebugFont( BitmapFont* font);
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
	void					SetObjectMatrixForBillBoard( Vec3 position ) const;
	
	void					SetWorldSize2D(const Vec2& worldMin, const Vec2& worldMax);
	Vec2					GetRelativePosInWorld2D(const Vec2& positionInWorld);

	Camera&					Get2DCamera();

	//------------------------------------------------------------------------------------------------------------------------------
	// 2D Debug Rendering (defaults to SCREEN)
	//------------------------------------------------------------------------------------------------------------------------------
	void				DebugRenderPoint2D( DebugRenderOptionsT options, const Vec2& position, float duration = 0.f, float size = DEFAULT_POINT_SIZE );
	void				DebugRenderLine2D( DebugRenderOptionsT options, const Vec2& start, const Vec2& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH );
	void				DebugRenderQuad2D( DebugRenderOptionsT options, AABB2 const &quad, float duration = 0.f, TextureView *view = nullptr); 
	void				DebugRenderWireQuad2D( DebugRenderOptionsT options, AABB2 const &quad, float duration = 0.f, float thickness = DEFAULT_WIRE_WIDTH_2D ); 
	void				DebugRenderDisc2D( DebugRenderOptionsT options, Disc2D const &disc, float duration = 0.f); 
	void				DebugRenderRing2D( DebugRenderOptionsT options, Disc2D const &disc, float duration = 0.f, float thickness = DEFAULT_DISC_THICKNESS ); 
	void				DebugRenderText2D( DebugRenderOptionsT options, const Vec2& startPosition, const Vec2& endPosition, char const *format, float fontHeight = DEFAULT_TEXT_HEIGHT, float duration = 0.f, ... );
	void				DebugRenderArrow2D( DebugRenderOptionsT options, const Vec2& start, const Vec2& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH ); 

	// Helpers
	void				DebugRenderPoint2D( const Vec2& position, float duration = 0.f, float size = DEFAULT_POINT_SIZE );
	void				DebugRenderLine2D( const Vec2& start, const Vec2& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH );
	void				DebugRenderQuad2D( AABB2 const &quad, float duration = 0.f, TextureView *view = nullptr); 
	void				DebugRenderWireQuad2D( AABB2 const &quad, float duration = 0.f, float thickness = DEFAULT_WIRE_WIDTH_2D ); 
	void				DebugRenderDisc2D( Disc2D const &disc, float duration = 0.f); 
	void				DebugRenderRing2D( Disc2D const &disc, float duration = 0.f, float thickness = DEFAULT_DISC_THICKNESS ); 
	void				DebugRenderText2D( const Vec2& startPosition, const Vec2& endPosition, char const *format, float fontHeight = DEFAULT_TEXT_HEIGHT, float duration = 0.f, ... );
	void				DebugRenderArrow2D( const Vec2& start, const Vec2& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH ); 


	//------------------------------------------------------------------------------------------------------------------------------
	// Text Logs
	//------------------------------------------------------------------------------------------------------------------------------
	void				DebugAddToLog( DebugRenderOptionsT options, char const* format, const Rgba& color = Rgba::ORANGE, float duration = 0.f, ...);
	void				DebugAddToLog( char const* format, const Rgba& color = Rgba::ORANGE, float duration = 0.f, ...);

	//------------------------------------------------------------------------------------------------------------------------------
	// 3D Rendering (will always default to WORLD)
	//------------------------------------------------------------------------------------------------------------------------------
	void				DebugRenderPoint( DebugRenderOptionsT options, const Vec3& position, float duration = 0.f, float size = DEFAULT_POINT_SIZE_3D, TextureView* texture = nullptr );
	void				DebugRenderLine( DebugRenderOptionsT options, const Vec3& start, const Vec3& end, float duration = 0.f, float lineWidth = DEFAULT_LINE_WIDTH_3D );
	void				DebugRenderSphere( DebugRenderOptionsT options, Vec3 center, float radius, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderBox( DebugRenderOptionsT options, const AABB3& box, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderQuad( DebugRenderOptionsT options, const AABB2& quad, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr, bool billBoarded = true);
	void				DebugRenderCapsule(DebugRenderOptionsT options, const Capsule3D& capsule, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr);

	// Helpers
	void				DebugRenderPoint( const Vec3& position, float duration = 0.f, float size = DEFAULT_POINT_SIZE_3D, TextureView* texture = nullptr );
	void				DebugRenderLine(const Vec3& start, const Vec3& end, float duration = 0.f);
	void				DebugRenderSphere( Vec3 center, float radius, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderBox( const AABB3& box, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderCapsule( const Capsule3D& capsule, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr);

	// EXTRA (helps to be able to set raster fill mode to "wire")
	// Also, better to use an ICOSphere if available, but UV sphere is fine; 
	void				DebugRenderWireSphere( DebugRenderOptionsT options, Vec3 center, float radius, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderWireBox( DebugRenderOptionsT options, const AABB3& box, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr ); 
	void				DebugRenderWireCapsule(DebugRenderOptionsT options, const Capsule3D& capsule, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr);

	// Helpers 
	void				DebugRenderWireCapsule(const Capsule3D& capsule, const Vec3& position, float duration = 0.f, TextureView* texture = nullptr);

	// EXTRA (requires being able to render a cone/cylinder)
	void				DebugRenderArrow( DebugRenderOptionsT options, Vec3 start, Vec3 end, float base_thickness, float head_thickness ); 

	// EXTRA - requires Arrow
	void				DebugRenderBasis( DebugRenderOptionsT options, Matrix44 const &basis, float lineWidth = DEFAULT_LINE_WIDTH ); 

	//------------------------------------------------------------------------------------------------------------------------------
	// Rendering 3D Text
	//------------------------------------------------------------------------------------------------------------------------------
	void				DebugRenderText3D( DebugRenderOptionsT options, const Vec3& position, const Vec2& pivot, char const *format, float fontHeight = DEFAULT_TEXT_HEIGHT_3D, float duration = 0.f, bool isBillboarded = true, ... );

private:
	IntVec2							ConvertWorldToScreenPoint(Vec3 worldPoint);	//Useless function here. Move this to Camera
	Vec3							ConvertScreenToWorldPoint(Vec3 screenPoint); //Same for this too

	
	void							DebugRenderToLog() const;				//This renders all the debug logs to the screen
	
	//Draw methods 2D
	void							DrawPoint2D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawLine2D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawQuad2D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawWireQuad2D		( const DebugRenderOptionsT* renderObject ) const;
	void							DrawDisc2D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawRing2D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawArrow2D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawText2D			( const DebugRenderOptionsT* renderObject ) const;

	//Draw methods 3D
	void							DrawPoint3D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawQuad3D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawLine3D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawSphere			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawWireSphere		( const DebugRenderOptionsT* renderObject )	const;
	void							DrawBox				( const DebugRenderOptionsT* renderObject ) const;
	void							DrawWireBox			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawText3D			( const DebugRenderOptionsT* renderObject ) const;
	void							DrawCapsule3D		(const DebugRenderOptionsT* renderObject) const;
	void							DrawWireCapsule3D	(const DebugRenderOptionsT* renderObject) const;

	//Destroy objects functions
	void							DestroyAllScreenObjects();
	void							DestroyAllWorldObjects();

private:
	const static Rgba						DEBUG_INFO;
	const static Rgba						DEBUG_ECHO;
	const static Rgba						DEBUG_BG_COLOR;

	const float								m_logFontHeight = DEFAULT_LOG_TEXT_HEIGHT;

	RenderContext*							m_renderContext					= nullptr;
	int										m_clientWidth					= 0;
	int										m_clientHeight					= 0;
	Camera*									m_debug3DCam					= nullptr;
	Camera*									m_debug2DCam					= nullptr;
	bool									m_canRender						= true;
	
	//2D world size
	Vec2									m_worldMin2D					= Vec2::ZERO;
	Vec2									m_worldMax2D					= Vec2::ZERO;

	BitmapFont*								m_debugFont						= nullptr;
	Shader*									m_xrayShader					= nullptr;
	Shader*									m_defaultShader					= nullptr;
	std::string								m_defaultShaderPath				= "default_unlit.xml";
	std::string								m_xmlShaderPath					= "default_unlit_xray.xml";

	//Keep a reference to the DebugRender instance for use with event systems
	static DebugRender*						s_debugRender;

	//Store all debug objects with their render options and other data
	std::vector<DebugRenderOptionsT>		m_worldRenderObjects;
	std::vector<DebugRenderOptionsT>		m_screenRenderObjects;

	std::vector<DebugRenderOptionsT>		m_printLogObjects;
};