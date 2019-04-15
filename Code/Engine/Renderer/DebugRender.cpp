//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include <cmath>
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
DebugRender* g_debugRenderer = nullptr;
DebugRender* DebugRender::s_debugRender = nullptr;

const STATIC Rgba DebugRender::DEBUG_INFO			=	Rgba::WHITE;
const STATIC Rgba DebugRender::DEBUG_ECHO			=	Rgba::YELLOW;
const STATIC Rgba DebugRender::DEBUG_BG_COLOR		=	Rgba(0.0f, 0.0f, 0.0f, 0.75f);

//------------------------------------------------------------------------------------------------------------------------------
DebugRender::DebugRender()
{

}

//------------------------------------------------------------------------------------------------------------------------------
DebugRender::~DebugRender()
{
	Shutdown();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::Startup( RenderContext* renderContext )
{
	m_renderContext = renderContext;

	if(s_debugRender == nullptr)
	{
		s_debugRender = this;
	}
	else
	{
		ERROR_AND_DIE("The static pointer to Debug Render was not null pointer");
	}

	//Get the Shader
	m_xrayShader = m_renderContext->CreateOrGetShaderFromFile(m_xmlShaderPath);
	m_xrayShader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);

	m_defaultShader = m_renderContext->CreateOrGetShaderFromFile(m_defaultShaderPath);
	m_defaultShader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);

	g_eventSystem->SubscribeEventCallBackFn("DisableDebug", DisableDebugRender);
	g_eventSystem->SubscribeEventCallBackFn("EnableDebug", EnableDebugRender);
	g_eventSystem->SubscribeEventCallBackFn("ClearAllDebug", ClearAllLiveObjects);
	g_eventSystem->SubscribeEventCallBackFn("ClearDebugScreen", ClearAllLiveScreenObjects);
	g_eventSystem->SubscribeEventCallBackFn("ClearDebugWorld", ClearAllLiveWorldObjects);
	
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::SetClientDimensions( int height, int width )
{
	m_clientHeight = height;
	m_clientWidth = width;


	//Add logic here to setup a default screen space camera which set to 720p or something
	m_debug2DCam = new Camera();
	float halfHeight = height * 0.5f;
	float halfWidth = width * 0.5f;
	m_debug2DCam->SetOrthoView(Vec2(-halfWidth, -halfHeight), Vec2(halfWidth, halfHeight));	
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::SetDebugFont( BitmapFont* font )
{
	m_debugFont = font;
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::Shutdown()
{
	delete m_debug2DCam;
	m_debug2DCam = nullptr;

	s_debugRender = nullptr;

	//TODO("Properly delete all the DebugRenderOptionsT objects stored in vectors");

	// No need to delete debug3DCam as we get this from Game camera. It is destroyed when Game is shutting down
	//delete m_debug3DCam;
	//m_debug3DCam = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::BeginFrame()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::EndFrame()
{
	CleanUpObjects();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::Update( float deltaTime )
{
	int vectorSize;
	float blendFraction;

	//Screen objects
	vectorSize = static_cast<int>(m_screenRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		ObjectProperties* objectProperties = m_screenRenderObjects[objectIndex].objectProperties;
		if(objectProperties->m_durationSeconds != 0.f)
		{
			//Subtract duration
			objectProperties->m_durationSeconds -= deltaTime;

			//Setup color value for the current time
			blendFraction = RangeMapFloat(objectProperties->m_durationSeconds, 0.f, objectProperties->m_startDuration, 0.f, 1.f);
			blendFraction = 1 - blendFraction;

			Rgba::LerpRGB( objectProperties->m_currentColor, m_screenRenderObjects[objectIndex].beginColor, m_screenRenderObjects[objectIndex].endColor, blendFraction );
		}
	}

	//World Objects
	vectorSize = static_cast<int>(m_worldRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		ObjectProperties* objectProperties = m_worldRenderObjects[objectIndex].objectProperties;
		if(objectProperties->m_durationSeconds != 0.f)
		{
			objectProperties->m_durationSeconds -= deltaTime;

			//Setup color value for the current time
			blendFraction = RangeMapFloat(objectProperties->m_durationSeconds, 0.f, objectProperties->m_startDuration, 0.f, 1.f);
			blendFraction = 1 - blendFraction;

			Rgba::LerpRGB( objectProperties->m_currentColor, m_worldRenderObjects[objectIndex].beginColor, m_worldRenderObjects[objectIndex].endColor, blendFraction );
			
			if(objectProperties->m_renderObjectType == DEBUG_RENDER_TEXT3D)
			{
				continue;
			}

			objectProperties->m_mesh->SetColor(objectProperties->m_currentColor);
		}
	}

	//Log Objects
	vectorSize = static_cast<int>(m_printLogObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		ObjectProperties* objectProperties = m_printLogObjects[objectIndex].objectProperties;
		if(objectProperties->m_durationSeconds != 0.f)
		{
			//Subtract duration
			objectProperties->m_durationSeconds -= deltaTime;

			//Setup color value for the current time
			blendFraction = RangeMapFloat(objectProperties->m_durationSeconds, 0.f, objectProperties->m_startDuration, 0.f, 1.f);
			blendFraction = 1 - blendFraction;

			Rgba::LerpRGB( objectProperties->m_currentColor, m_printLogObjects[objectIndex].beginColor, m_printLogObjects[objectIndex].endColor, blendFraction );
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::CleanUpObjects()
{
	//TODO("Call Delete on all the ObjectProperties and also add destructor to the DebugRenderOptionsT object");

	//Screen 
	for(int objectIndex = 0; objectIndex < (int)m_screenRenderObjects.size(); objectIndex++)
	{
		if(m_screenRenderObjects[objectIndex].objectProperties->m_durationSeconds <= 0.f)
		{
			delete m_screenRenderObjects[objectIndex].objectProperties;
			m_screenRenderObjects[objectIndex].objectProperties = nullptr;
			m_screenRenderObjects.erase(m_screenRenderObjects.begin() + objectIndex);
		}
	}

	//World
	for(int objectIndex = 0; objectIndex < (int)m_worldRenderObjects.size(); objectIndex++)
	{
		if(m_worldRenderObjects[objectIndex].objectProperties->m_durationSeconds <= 0.f)
		{
			delete m_worldRenderObjects[objectIndex].objectProperties;
			m_worldRenderObjects[objectIndex].objectProperties = nullptr;
			m_worldRenderObjects.erase(m_worldRenderObjects.begin() + objectIndex);
		}
	}

	//Log
	for(int objectIndex = 0; objectIndex < (int)m_printLogObjects.size(); objectIndex++)
	{
		if(m_printLogObjects[objectIndex].objectProperties->m_durationSeconds <= 0.f)
		{
			delete m_printLogObjects[objectIndex].objectProperties;
			m_printLogObjects[objectIndex].objectProperties = nullptr;
			m_printLogObjects.erase(m_printLogObjects.begin() + objectIndex);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderToScreen() const
{
	if(!m_canRender)
	{
		DebugRenderToLog();
		return;
	}

	//Use this method to render to screen camera
	int vectorSize = static_cast<int>(m_screenRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		const DebugRenderOptionsT* renderObject = &m_screenRenderObjects[objectIndex];

		//Render the object for this frame
		switch(renderObject->objectProperties->m_renderObjectType)
		{
		case DEBUG_RENDER_POINT:		{	DrawPoint2D( renderObject);		}	break;
		case DEBUG_RENDER_LINE:			{	DrawLine2D(renderObject);		}	break;
		case DEBUG_RENDER_QUAD:			{	DrawQuad2D(renderObject);		}	break;
		case DEBUG_RENDER_DISC:			{	DrawDisc2D(renderObject);		}	break;
		case DEBUG_RENDER_RING:			{	DrawRing2D(renderObject);		}	break;
		case DEBUG_RENDER_WIRE_QUAD:	{	DrawWireQuad2D(renderObject);	}	break;
		case DEBUG_RENDER_TEXT:			{	DrawText2D(renderObject);		}	break;
		case DEBUG_RENDER_ARROW:		{	DrawArrow2D(renderObject);		}	break;
		default:						{	ERROR_AND_DIE("The debug object is not yet defined in DebugRenderToScreen");	}	break;
		}

		//If the duration is 0.f or lower, set the object to nullptr
		if(renderObject->objectProperties->m_durationSeconds <= 0.f)
		{
			//Set the object to nullptr
			renderObject = nullptr;
		}
	}

	DebugRenderToLog();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderToCamera() const
{
	if(!m_canRender)
	{
		return;
	}

	//Use this method to render to the world camera
	int vectorSize = static_cast<int>(m_worldRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		const DebugRenderOptionsT* renderObject = &m_worldRenderObjects[objectIndex];
		switch(renderObject->objectProperties->m_renderObjectType)
		{
		case DEBUG_RENDER_POINT3D:		{	DrawPoint3D(renderObject);		}	break;
		case DEBUG_RENDER_LINE3D:		{	DrawLine3D(renderObject);		}	break;
		case DEBUG_RENDER_SPHERE:		{	DrawSphere(renderObject);		}	break;
		case DEBUG_RENDER_BOX:			{	DrawBox(renderObject);			}	break;
		case DEBUG_RENDER_QUAD3D:		{	DrawQuad3D(renderObject);		}	break;
		case DEBUG_RENDER_WIRE_SPHERE:	{	DrawWireSphere(renderObject);	}	break;
		case DEBUG_RENDER_WIRE_BOX:		{	DrawWireBox(renderObject);		}	break;
		case DEBUG_RENDER_TEXT3D:		{	DrawText3D(renderObject);		}	break;
		default:						{	ERROR_AND_DIE("The debug object is not yet defined in DebugRenderToCamera");	}	break;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderToLog() const
{
	Setup2DCamera();

	m_renderContext->BindTextureViewWithSampler(0U, m_debugFont->GetTexture());

	//Draw a black box over the entire screen
	AABB2 blackBox = AABB2(m_debug2DCam->GetOrthoBottomLeft(), m_debug2DCam->GetOrthoTopRight());
	std::vector<Vertex_PCU> boxVerts;
	AddVertsForAABB2D(boxVerts, blackBox, DEBUG_BG_COLOR);

	//Set the text based on Camera size
	Vec2 leftBot = m_debug2DCam->GetOrthoBottomLeft();
	Vec2 rightTop = m_debug2DCam->GetOrthoTopRight();

	float screenHeight = rightTop.y - leftBot.y;
	//float screenWidth = rightTop.x - leftBot.x;

	int numLines = static_cast<int>(screenHeight / m_logFontHeight);
	int numStrings = static_cast<int>(m_printLogObjects.size());

	Vec2 boxBottomLeft = leftBot + Vec2(m_logFontHeight, (numLines) * m_logFontHeight);
	Vec2 boxTopRight = boxBottomLeft;

	//Get the last string in the map and work your way back
	std::vector<DebugRenderOptionsT>::const_iterator vecIterator = m_printLogObjects.end();

	std::vector<Vertex_PCU> textVerts;

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
		LogProperties* objectProperties = reinterpret_cast<LogProperties*>(m_printLogObjects[lineIndex].objectProperties);
		if(objectProperties->m_renderObjectType != DEBUG_RENDER_LOG)
		{
			ERROR_AND_DIE("Object recieved in DebugRender was not log text. Check inputs");
		}

		//Decrement the iterator
		vecIterator--;

		//Get length of string
		std::string printString = "[ Start Time:" + Stringf("%01f", objectProperties->m_startDuration) 
								+ " Time Left:" + std::to_string(objectProperties->m_durationSeconds) + " ] " 
								+ objectProperties->m_string;
		int numChars = static_cast<int>(printString.length());

		//Create the required box
		float glyphWidth = m_logFontHeight * m_debugFont->GetGlyphAspect(0);

		//Change the boxBottomLeft based on line height
		boxBottomLeft.y -= m_logFontHeight;
		boxTopRight = boxBottomLeft;

		boxTopRight = boxTopRight + Vec2(numChars * glyphWidth , m_logFontHeight);
		
		AABB2 printBox = AABB2(boxBottomLeft, boxTopRight);

		//Print the text
		m_debugFont->AddVertsForTextInBox2D(textVerts, printBox, m_logFontHeight, printString, objectProperties->m_currentColor, m_debugFont->GetGlyphAspect(0), Vec2::ALIGN_LEFT_CENTERED);

		//Add a padding if desired
		//boxBottomLeft.y += CONSOLE_LINE_SPACE;
	}

	m_renderContext->DrawVertexArray(textVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
Camera& DebugRender::Get2DCamera()
{
	return *m_debug2DCam;
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawPoint2D( const DebugRenderOptionsT* renderObject) const
{
	Setup2DCamera();

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Point2DProperties* objectProperties = reinterpret_cast<Point2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_POINT)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a point. Check inputs");
	}

	//TODO: Implement code to handle duration logic
	Vec2 minBounds = Vec2(objectProperties->m_size * -0.5f, objectProperties->m_size * -0.5f);
	Vec2 maxBounds = Vec2(objectProperties->m_size * 0.5f, objectProperties->m_size * 0.5f);

	minBounds += objectProperties->m_screenPosition;
	maxBounds += objectProperties->m_screenPosition;

	AABB2 box = AABB2(minBounds, maxBounds);
	std::vector<Vertex_PCU> pointVerts;
	AddVertsForAABB2D(pointVerts, box, objectProperties->m_currentColor );

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(pointVerts);

}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::Setup2DCamera() const
{
	m_debug2DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug2DCam->UpdateUniformBuffer(m_renderContext);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::Setup3DCamera(Camera* const debugCamera)
{
	m_debug3DCam = debugCamera;
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawLine2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Line2DProperties* objectProperties = reinterpret_cast<Line2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_LINE)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Line. Check inputs");
	}

	//TODO: Implement code to handle duration logic
	std::vector<Vertex_PCU> lineVerts;
	AddVertsForLine2D(lineVerts, objectProperties->m_startPos, objectProperties->m_endPos, objectProperties->m_lineWidth,  objectProperties->m_currentColor);

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(lineVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawQuad2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	Quad2DProperties* objectProperties = reinterpret_cast<Quad2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_QUAD)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Quad. Check inputs");
	}

	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture);

	std::vector<Vertex_PCU> boxVerts;
	AddVertsForAABB2D(boxVerts, objectProperties->m_quad, renderObject->objectProperties->m_currentColor);

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(boxVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawWireQuad2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	Quad2DProperties* objectProperties = reinterpret_cast<Quad2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_WIRE_QUAD)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Wire Quad. Check inputs");
	}

	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture);

	std::vector<Vertex_PCU> boxVerts;
	AddVertsForBoundingBox(boxVerts, objectProperties->m_quad, objectProperties->m_currentColor, objectProperties->m_thickness);

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(boxVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawDisc2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	Disc2DProperties* objectProperties = reinterpret_cast<Disc2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_DISC)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Disc. Check inputs");
	}

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	std::vector<Vertex_PCU> ringVerts;
	//AddVertsForAABB2D(boxVerts, objectProperties->m_quad, renderObject->objectProperties->m_currentColor);
	AddVertsForDisc2D(ringVerts, objectProperties->m_disc.GetCentre(), objectProperties->m_disc.GetRadius(), objectProperties->m_currentColor);

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(ringVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawRing2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	Disc2DProperties* objectProperties = reinterpret_cast<Disc2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_RING)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Ring. Check inputs");
	}

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	std::vector<Vertex_PCU> ringVerts;
	//AddVertsForAABB2D(boxVerts, objectProperties->m_quad, renderObject->objectProperties->m_currentColor);
	AddVertsForRing2D(ringVerts, objectProperties->m_disc.GetCentre(), objectProperties->m_disc.GetRadius(), objectProperties->m_thickness, objectProperties->m_currentColor);

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(ringVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawArrow2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Arrow2DProperties* objectProperties = reinterpret_cast<Arrow2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_ARROW)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not an Arrow. Check inputs");
	}

	//TODO: Implement code to handle duration logic
	std::vector<Vertex_PCU> arrowVerts;
	
	Vec2 lineStart = objectProperties->m_startPos;
	Vec2 lineEnd = objectProperties->m_lineEnd;

	AddVertsForLine2D(arrowVerts, lineStart, lineEnd, objectProperties->m_lineWidth,  objectProperties->m_currentColor);

	Vec3 arrowTip3D = Vec3(objectProperties->m_arrowTip);
	Vec2 lineTangent = objectProperties->m_lineNorm.GetRotated90Degrees();
	Vec2 arrowLeft = lineEnd + (lineTangent * objectProperties->m_lineWidth * 2.f);
	Vec2 arrowRight = lineEnd - (lineTangent * objectProperties->m_lineWidth * 2.f);

	Vec3 arrowLeft3D = Vec3(arrowLeft);
	Vec3 arrowRight3D = Vec3(arrowRight);

	arrowVerts.push_back(Vertex_PCU(arrowRight3D, objectProperties->m_currentColor, Vec2::ZERO));
	arrowVerts.push_back(Vertex_PCU(arrowTip3D, objectProperties->m_currentColor, Vec2::ZERO));
	arrowVerts.push_back(Vertex_PCU(arrowLeft3D, objectProperties->m_currentColor, Vec2::ZERO));

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(arrowVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawText2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	TextProperties* objectProperties = reinterpret_cast<TextProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_TEXT)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not text. Check inputs");
	}

	m_renderContext->BindTextureViewWithSampler(0U, m_debugFont->GetTexture());

	std::vector<Vertex_PCU> textVerts;

	AABB2 textBox = AABB2(objectProperties->m_startPosition - Vec2(0.f, objectProperties->m_fontHeight * 0.5f), objectProperties->m_endPosition + Vec2(0.f, objectProperties->m_fontHeight * 0.f));
	m_debugFont->AddVertsForTextInBox2D(textVerts, textBox, objectProperties->m_fontHeight, objectProperties->m_string, objectProperties->m_currentColor);

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	}

	m_renderContext->DrawVertexArray(textVerts);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawPoint3D( const DebugRenderOptionsT* renderObject ) const
{
	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Point3DProperties* objectProperties = reinterpret_cast<Point3DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_POINT3D)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a 3D Point. Check inputs");
	}

	//Setup mesh here
	GPUMesh point = GPUMesh( m_renderContext ); 
	point.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForBillBoard(objectProperties->m_position);


	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&point);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&point);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&point);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&point);
		}
		break;
	}

}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawQuad3D( const DebugRenderOptionsT* renderObject ) const
{
	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Quad3DProperties* objectProperties = reinterpret_cast<Quad3DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_QUAD3D)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a 3D Quad. Check inputs");
	}

	//Setup mesh here
	GPUMesh quad = GPUMesh( m_renderContext ); 
	quad.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	if(objectProperties->m_billBoarded)
	{
		SetObjectMatrixForBillBoard(objectProperties->m_position);
	}
	else
	{
		SetObjectMatrixForPosition(objectProperties->m_position);
	}

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&quad);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&quad);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&quad);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&quad);
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawLine3D( const DebugRenderOptionsT* renderObject ) const
{
	Line3DProperties* objectProperties = reinterpret_cast<Line3DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_LINE3D)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a 3D Line. Check inputs");
	}

	//Setup mesh here
	GPUMesh line = GPUMesh( m_renderContext ); 
	line.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_center);
	//SetObjectMatrixForBillBoard(objectProperties->m_center);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, nullptr); 

	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&line);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&line);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&line);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&line);
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawSphere( const DebugRenderOptionsT* renderObject ) const
{
	SphereProperties* objectProperties = reinterpret_cast<SphereProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_SPHERE)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Sphere. Check inputs");
	}

	//Setup mesh here
	GPUMesh sphere = GPUMesh( m_renderContext ); 
	sphere.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_center);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&sphere);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&sphere);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&sphere);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&sphere);
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawWireSphere( const DebugRenderOptionsT* renderObject ) const
{
	SphereProperties* objectProperties = reinterpret_cast<SphereProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_WIRE_SPHERE)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Sphere. Check inputs");
	}

	//Setup mesh here
	GPUMesh sphere = GPUMesh( m_renderContext ); 
	sphere.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_center);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	m_renderContext->SetRasterStateWireFrame();
	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&sphere);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&sphere);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&sphere);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&sphere);
		}
		break;
	}
	m_renderContext->CreateAndSetDefaultRasterState();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawBox( const DebugRenderOptionsT* renderObject ) const
{
	BoxProperties* objectProperties = reinterpret_cast<BoxProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_BOX)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Box. Check inputs");
	}

	//Setup mesh here
	GPUMesh box = GPUMesh( m_renderContext ); 
	box.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_position);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&box);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&box);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&box);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&box);
		}
	break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawWireBox( const DebugRenderOptionsT* renderObject ) const
{
	BoxProperties* objectProperties = reinterpret_cast<BoxProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_WIRE_BOX)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Box. Check inputs");
	}

	//Setup mesh here
	GPUMesh box = GPUMesh( m_renderContext ); 
	box.CreateFromCPUMesh<Vertex_PCU>( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_position);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	m_renderContext->SetRasterStateWireFrame();
	switch (renderObject->mode)
	{
		case DEBUG_RENDER_USE_DEPTH:
		m_renderContext->SetDepth(true);
		m_renderContext->DrawMesh(&box);
		break;
		case DEBUG_RENDER_ALWAYS:
		m_renderContext->SetDepth(false);
		m_renderContext->DrawMesh(&box);
		break;
		case DEBUG_RENDER_XRAY:
		{
			//Make 2 draw calls here
			//One with compare op lequals and one with compare op greater than (edit alpha on that one)
			m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
			m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
			m_renderContext->BindShader(m_xrayShader);
			m_renderContext->DrawMesh(&box);
			m_renderContext->BindShader(m_defaultShader);
			m_renderContext->DrawMesh(&box);
		}
		break;
	}
	m_renderContext->CreateAndSetDefaultRasterState();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DrawText3D( const DebugRenderOptionsT* renderObject ) const
{
	TextProperties* objectProperties = reinterpret_cast<TextProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_TEXT3D)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not 3D Text. Check inputs");
	}

	std::vector<Vertex_PCU> textVerts;

	int numChars = (int)objectProperties->m_string.size();

	Vec3 mins = objectProperties->m_position;
	Vec3 maxs = objectProperties->m_position;
	
	mins -= Vec3(objectProperties->m_pivot.x * (float)numChars * objectProperties->m_fontHeight, objectProperties->m_fontHeight * objectProperties->m_pivot.y * 0.5f, 0.f);
	maxs += Vec3(objectProperties->m_pivot.x * (float)numChars * objectProperties->m_fontHeight, objectProperties->m_fontHeight * objectProperties->m_pivot.y * 0.5f, 0.f);

	AABB2 lineBox = AABB2(mins, maxs);
	m_debugFont->AddVertsForTextInBox3D(textVerts, lineBox, maxs.y - mins.y, objectProperties->m_string, objectProperties->m_currentColor);

	if(objectProperties->m_isBillboarded)
	{
		SetObjectMatrixForBillBoard(objectProperties->m_position);
	}
	else
	{
		SetObjectMatrixForPosition(objectProperties->m_position);
	}

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, m_debugFont->GetTexture()); 

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	m_renderContext->DrawVertexArray(textVerts);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	m_renderContext->DrawVertexArray(textVerts);
	break;
	case DEBUG_RENDER_XRAY:
	m_renderContext->SetGlobalTint(Rgba::DARK_GREY);
	m_xrayShader->SetDepth(eCompareOp::COMPARE_GREATER, false);
	m_renderContext->BindShader(m_xrayShader);
	m_renderContext->DrawVertexArray(textVerts);
	m_renderContext->BindShader(m_defaultShader);
	m_renderContext->DrawVertexArray(textVerts);
	break;
	}

}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DestroyAllScreenObjects()
{
	m_screenRenderObjects.clear();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DestroyAllWorldObjects()
{
	m_worldRenderObjects.clear();
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::SetObjectMatrixForPosition( Vec3 position ) const
{
	//Setup matrix for position
	Matrix44 objectMatrix = Matrix44::MakeFromEuler(Vec3::ZERO);
	Matrix44::SetTranslation3D(position, objectMatrix);

	//Set on Render Context
	m_renderContext->SetModelMatrix( objectMatrix ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::SetObjectMatrixForBillBoard( Vec3 position ) const
{
	Matrix44 cameraModel = m_debug3DCam->GetModelMatrix();
	Matrix44 objectModel = Matrix44::IDENTITY;
	objectModel.SetRotationFromMatrix(objectModel, cameraModel);

	objectModel = Matrix44::SetTranslation3D(position, objectModel);

	m_renderContext->SetModelMatrix(objectModel);	
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::SetWorldSize2D( const Vec2& worldMin, const Vec2& worldMax )
{
	m_worldMin2D = worldMin;
	m_worldMax2D = worldMax;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 DebugRender::GetRelativePosInWorld2D( const Vec2& positionInWorld )
{
	Vec2 correctedPos;
	correctedPos.x = RangeMapFloat(positionInWorld.x, m_worldMin2D.x, m_worldMax2D.x, m_debug2DCam->GetOrthoBottomLeft().x, m_debug2DCam->GetOrthoTopRight().x);
	correctedPos.y = RangeMapFloat(positionInWorld.y, m_worldMin2D.y, m_worldMax2D.y, m_debug2DCam->GetOrthoBottomLeft().y, m_debug2DCam->GetOrthoTopRight().y);
	return correctedPos;
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderPoint2D( DebugRenderOptionsT options, const Vec2& position, float duration, float size /*= DEFAULT_POINT_SIZE */ )
{
	if(options.relativeCoordinates)
	{
		Vec2 newPosition = GetRelativePosInWorld2D(position);
		options.objectProperties = new Point2DProperties(DEBUG_RENDER_POINT, newPosition, duration, size);
	}
	else
	{
		options.objectProperties = new Point2DProperties(DEBUG_RENDER_POINT, position, duration, size);
	}

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderLine2D(DebugRenderOptionsT options, const Vec2& start, const Vec2& end, float duration, float lineWidth)
{
	options.objectProperties = new Line2DProperties(DEBUG_RENDER_LINE, start, end, duration, lineWidth);

	m_screenRenderObjects.push_back(options);

}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderQuad2D( DebugRenderOptionsT options, AABB2 const & quad, float duration, TextureView* texture )
{
	options.objectProperties = new Quad2DProperties(DEBUG_RENDER_QUAD, quad, duration, DEFAULT_WIRE_WIDTH_2D, texture);

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderWireQuad2D( DebugRenderOptionsT options, AABB2 const &quad, float duration /*= 0.f*/, float thickness /*= DEFAULT_WIRE_WIDTH_2D */ )
{
	options.objectProperties = new Quad2DProperties(DEBUG_RENDER_WIRE_QUAD, quad, duration, thickness);

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
// This function is currently useless (For now)
//------------------------------------------------------------------------------------------------------------------------------
IntVec2 DebugRender::ConvertWorldToScreenPoint( Vec3 worldPoint)
{
	//Get the view projection matrix
	Matrix44 projectionMat = m_renderContext->GetProjectionMatrix();
	Matrix44 viewMat = m_renderContext->GetViewMatrix();

	Matrix44 viewProjMat = projectionMat.AppendMatrix(viewMat);

	//Transform world to clip coordinates
	Vec3 point3D = viewProjMat.TransformPosition3D(worldPoint);

	//Calculate wrt NDC
	int winX = (int)round(((point3D.x + 1) / 2.0f) * m_clientWidth);
	int winY = (int)round(((1 - point3D.x) / 2.0f) * m_clientHeight);

	return IntVec2(winX, winY);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderDisc2D( DebugRenderOptionsT options, Disc2D const &disc, float duration /*= 0.f*/)
{
	options.objectProperties = new Disc2DProperties(DEBUG_RENDER_DISC, disc, 0.f, duration);

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderRing2D( DebugRenderOptionsT options, Disc2D const &disc, float duration /*= 0.f*/, float thickness /*= DEFAULT_DISC_THICKNESS */ )
{
	options.objectProperties = new Disc2DProperties(DEBUG_RENDER_RING, disc, thickness, duration);

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderText2D( DebugRenderOptionsT options, const Vec2& startPosition, const Vec2& endPosition, char const *format, float fontHeight /*= DEFAULT_TEXT_HEIGHT*/, float duration /*= 0.f*/, ... )
{
	char buffer[1024]; 

	va_list args;
	va_start( args, format ); 
	vsprintf_s( buffer, 1024, format, args ); 
	va_end( args ); 

	buffer[ 1024 - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	std::string text = std::string( buffer);

	options.objectProperties = new TextProperties(DEBUG_RENDER_TEXT, startPosition, endPosition, text, fontHeight, duration);

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderArrow2D( DebugRenderOptionsT options, const Vec2& start, const Vec2& end, float duration /*= 0.f*/, float lineWidth /*= DEFAULT_LINE_WIDTH */ )
{
	if(options.relativeCoordinates)
	{
		Vec2 newStart = GetRelativePosInWorld2D(start);
		Vec2 newEnd = GetRelativePosInWorld2D(end);
		options.objectProperties = new Arrow2DProperties(DEBUG_RENDER_ARROW, newStart, newEnd, duration, lineWidth);
	}
	else
	{
		options.objectProperties = new Arrow2DProperties(DEBUG_RENDER_ARROW, start, end, duration, lineWidth);
	}

	m_screenRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugAddToLog( DebugRenderOptionsT options, char const* format, const Rgba& color, float duration, ... )
{
	char buffer[1024]; 

	va_list args;
	va_start( args, duration ); 
	vsprintf_s( buffer, 1024, format, args ); 
	va_end( args ); 

	buffer[ 1024 - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	std::string text = std::string( buffer);

	options.objectProperties = new LogProperties(DEBUG_RENDER_LOG, color, text, duration);

	m_printLogObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderPoint( DebugRenderOptionsT options, const Vec3& position, float duration, float size, TextureView* texture )
{
	options.objectProperties = new Point3DProperties(DEBUG_RENDER_POINT3D, position, size, duration, texture);

	m_worldRenderObjects.push_back(options);
}
 
//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderLine( DebugRenderOptionsT options, const Vec3& start, const Vec3& end, float duration, float lineWidth /*= DEFAULT_LINE_WIDTH */ )
{
	options.objectProperties = new Line3DProperties(DEBUG_RENDER_LINE3D, start, end, duration, lineWidth);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderSphere( DebugRenderOptionsT options, Vec3 center, float radius, float duration /*= 0.f*/, TextureView* texture /*= nullptr */ )
{
	options.objectProperties = new SphereProperties(DEBUG_RENDER_SPHERE, center, radius, duration, texture);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderBox( DebugRenderOptionsT options, const AABB3& box, const Vec3& position, float duration /*= 0.f*/, TextureView* texture /*= nullptr */ )
{
	options.objectProperties = new BoxProperties(DEBUG_RENDER_BOX, box, position, duration, texture);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderQuad( DebugRenderOptionsT options, const AABB2& quad, const Vec3& position, float duration /*= 0.f*/, TextureView* texture /*= nullptr*/, bool billBoarded /* = true */ )
{
	options.objectProperties = new Quad3DProperties(DEBUG_RENDER_QUAD3D, quad, position, duration, texture, billBoarded);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderWireSphere( DebugRenderOptionsT options, Vec3 center, float radius, float duration /*= 0.f*/, TextureView* texture /*= nullptr */ )
{
	options.objectProperties = new SphereProperties(DEBUG_RENDER_WIRE_SPHERE, center, radius, duration, texture);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderWireBox( DebugRenderOptionsT options, const AABB3& box, const Vec3& position, float duration /*= 0.f*/, TextureView* texture /*= nullptr */ )
{
	options.objectProperties = new BoxProperties(DEBUG_RENDER_WIRE_BOX, box, position, duration, texture);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
void DebugRender::DebugRenderText3D( DebugRenderOptionsT options, const Vec3& position, const Vec2& pivot, char const *format, float fontHeight, float duration, bool isBillboarded, ... )
{
	char buffer[1024]; 

	va_list args;
	va_start( args, isBillboarded ); 
	vsprintf_s( buffer, 1024, format, args ); 
	va_end( args ); 

	buffer[ 1024 - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	std::string text = std::string( buffer);

	options.objectProperties = new TextProperties(DEBUG_RENDER_TEXT3D, position, pivot, text, fontHeight, duration, isBillboarded);

	m_worldRenderObjects.push_back(options);
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool DebugRender::DisableDebugRender(EventArgs& args)
{
	UNUSED(args);
	s_debugRender->m_canRender = false;
	g_devConsole->PrintString(Rgba::YELLOW, "Disabling all debug objects");
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool DebugRender::EnableDebugRender( EventArgs& args )
{
	UNUSED(args);
	s_debugRender->m_canRender = true;
	g_devConsole->PrintString(Rgba::YELLOW, "Enabling all debug objects");
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool DebugRender::ClearAllLiveObjects( EventArgs& args )
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "Destroying all Screen Objects");
	s_debugRender->DestroyAllScreenObjects();
	g_devConsole->PrintString(Rgba::GREEN, "Screen Objects Destroyed");
	g_devConsole->PrintString(Rgba::YELLOW, "Destroying all World Objects");
	s_debugRender->DestroyAllWorldObjects();
	g_devConsole->PrintString(Rgba::GREEN, "World Objects Destroyed");
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool DebugRender::ClearAllLiveScreenObjects( EventArgs& args )
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "Destroying all Screen Objects");
	s_debugRender->DestroyAllScreenObjects();
	g_devConsole->PrintString(Rgba::GREEN, "Screen Objects Destroyed");
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool DebugRender::ClearAllLiveWorldObjects( EventArgs& args )
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "Destroying all World Objects");
	s_debugRender->DestroyAllWorldObjects();
	g_devConsole->PrintString(Rgba::GREEN, "World Objects Destroyed");
	return true;
}
