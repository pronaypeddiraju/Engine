//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <cmath>
#include <vector>

DebugRender* g_debugRenderer = nullptr;

DebugRender::DebugRender()
{

}

DebugRender::~DebugRender()
{
	Shutdown();
}

void DebugRender::Startup( RenderContext* renderContext )
{
	m_renderContext = renderContext;
}


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

void DebugRender::Shutdown()
{
	delete m_debug2DCam;
	m_debug2DCam = nullptr;

	// No need to delete debug3DCam as we get this from Game camera. It is destroyed when Game is shutting down
	//delete m_debug3DCam;
	//m_debug3DCam = nullptr;
}

void DebugRender::BeginFrame()
{

}

void DebugRender::EndFrame()
{
	CleanUpObjects();
}

void DebugRender::Update( float deltaTime )
{
	int vectorSize;
	float blendFraction;

	vectorSize = static_cast<int>(screenRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		ObjectProperties* objectProperties = screenRenderObjects[objectIndex].objectProperties;
		if(objectProperties->m_durationSeconds != 0.f)
		{
			//Subtract duration
			objectProperties->m_durationSeconds -= deltaTime;

			//Setup color value for the current time
			blendFraction = RangeMapFloat(objectProperties->m_durationSeconds, 0.f, objectProperties->m_startDuration, 0.f, 1.f);
			blendFraction = 1 - blendFraction;

			Rgba::LerpRGB( objectProperties->m_currentColor, screenRenderObjects[objectIndex].beginColor, screenRenderObjects[objectIndex].endColor, blendFraction );
		}
	}

	vectorSize = static_cast<int>(worldRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		ObjectProperties* objectProperties = worldRenderObjects[objectIndex].objectProperties;
		if(objectProperties->m_durationSeconds != 0.f)
		{
			objectProperties->m_durationSeconds -= deltaTime;

			//Setup color value for the current time
			blendFraction = RangeMapFloat(objectProperties->m_durationSeconds, 0.f, objectProperties->m_startDuration, 0.f, 1.f);
			blendFraction = 1 - blendFraction;

			Rgba::LerpRGB( objectProperties->m_currentColor, worldRenderObjects[objectIndex].beginColor, worldRenderObjects[objectIndex].endColor, blendFraction );
		}
	}

}

void DebugRender::CleanUpObjects()
{
	for(int objectIndex = 0; objectIndex < (int)screenRenderObjects.size(); objectIndex++)
	{
		if(screenRenderObjects[objectIndex].objectProperties->m_durationSeconds <= 0.f)
		{
			screenRenderObjects.erase(screenRenderObjects.begin() + objectIndex);
		}
	}

	for(int objectIndex = 0; objectIndex < (int)worldRenderObjects.size(); objectIndex++)
	{
		if(worldRenderObjects[objectIndex].objectProperties->m_durationSeconds <= 0.f)
		{
			worldRenderObjects.erase(worldRenderObjects.begin() + objectIndex);
		}
	}

}

void DebugRender::DebugRenderToScreen() const
{
	//Use this method to render to screen camera
	int vectorSize = static_cast<int>(screenRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		const DebugRenderOptionsT* renderObject = &screenRenderObjects[objectIndex];

		//Render the object for this frame
		switch(renderObject->objectProperties->m_renderObjectType)
		{
		case DEBUG_RENDER_POINT:		{	DrawPoint2D( renderObject);		}	break;
		case DEBUG_RENDER_LINE:			{	DrawLine2D(renderObject);		}	break;
		case DEBUG_RENDER_QUAD:			{	DrawQuad2D(renderObject);		}	break;
		default:						{	ERROR_AND_DIE("The debug object is not yet defined in DebugRenderToScreen");	}	break;
		}

		//If the duration is 0.f or lower, set the object to nullptr
		if(renderObject->objectProperties->m_durationSeconds <= 0.f)
		{
			//Set the object to nullptr
			renderObject = nullptr;
		}
	}
}

void DebugRender::DebugRenderToCamera() const
{
	//Use this method to render to the world camera
	int vectorSize = static_cast<int>(worldRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		const DebugRenderOptionsT* renderObject = &worldRenderObjects[objectIndex];
		switch(renderObject->objectProperties->m_renderObjectType)
		{
		case DEBUG_RENDER_POINT3D:		{	DrawPoint3D(renderObject);		}	break;
		default:						{	GUARANTEE_OR_DIE(true, "The debug object is not yet defined in DebugRenderToCamera");	}	break;
		}
	}
}

Camera& DebugRender::Get2DCamera()
{
	return *m_debug2DCam;
}

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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(pointVerts);

}

void DebugRender::Setup2DCamera() const
{
	m_debug2DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug2DCam->UpdateUniformBuffer(m_renderContext);
}

void DebugRender::Setup3DCamera(Camera* const debugCamera)
{
	m_debug3DCam = debugCamera;
}

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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(lineVerts);
}

void DebugRender::DrawQuad2D( const DebugRenderOptionsT* renderObject ) const
{
	Setup2DCamera();

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Quad2DProperties* objectProperties = reinterpret_cast<Quad2DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_QUAD)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Quad. Check inputs");
	}

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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(boxVerts);
}

void DebugRender::DrawPoint3D( const DebugRenderOptionsT* renderObject ) const
{
	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Point3DProperties* objectProperties = reinterpret_cast<Point3DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_POINT3D)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a 3D Point. Check inputs");
	}

	std::vector<Vertex_PCU> pointVerts;
	Vec2 boxMins = Vec2((objectProperties->m_position.x - objectProperties->m_size * 0.5f), (objectProperties->m_position.y - objectProperties->m_size * 0.5f));
	Vec2 boxMaxs = Vec2((objectProperties->m_position.x + objectProperties->m_size * 0.5f), (objectProperties->m_position.y + objectProperties->m_size * 0.5f));
	AABB2 pointBox = AABB2(boxMins, boxMaxs);


	SetObjectMatrixForPosition(objectProperties->m_position);
	AddVertsForAABB2D(pointVerts, pointBox, objectProperties->m_currentColor);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, objectProperties->m_texture); 

	switch (renderObject->mode)
	{
	case DEBUG_RENDER_USE_DEPTH:
	m_renderContext->SetDepth(true);
	break;
	case DEBUG_RENDER_ALWAYS:
	m_renderContext->SetDepth(false);
	break;
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(pointVerts);

}

void DebugRender::SetObjectMatrixForPosition( Vec3 position ) const
{
	m_debug3DCam->UpdateUniformBuffer(m_renderContext);

	//Setup matrix for position
	Matrix44 pointTransform = m_debug3DCam->GetModelMatrix();
	Vec3 euler = m_debug3DCam->GetEuler();
	Matrix44 objectMatrix = Matrix44::MakeFromEuler(euler);
	Matrix44::SetTranslation3D(position, objectMatrix);

	//Set on Render Context
	m_renderContext->SetModelMatrix( objectMatrix ); 
}

void DebugRender::DebugRenderPoint2D( DebugRenderOptionsT options, Vec2 position, float duration, float size /*= DEFAULT_POINT_SIZE */ )
{
	options.objectProperties = new Point2DProperties(DEBUG_RENDER_POINT, position, duration, size);

	screenRenderObjects.push_back(options);
}

//Inplementation to debug render a line using 2D debug camera
void DebugRender::DebugRenderLine2D(DebugRenderOptionsT options, Vec2 start, Vec2 end, float duration, float lineWidth)
{
	options.objectProperties = new Line2DProperties(DEBUG_RENDER_LINE, start, end, duration, lineWidth);

	screenRenderObjects.push_back(options);

}

void DebugRender::DebugRenderQuad2D( DebugRenderOptionsT options, AABB2 const & quad, float duration )
{
	options.objectProperties = new Quad2DProperties(DEBUG_RENDER_QUAD, quad, duration);

	screenRenderObjects.push_back(options);
}

// This function is currently useless
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

void DebugRender::DebugRenderPoint( DebugRenderOptionsT options, const Vec3& position, float duration, float size, TextureView* texture )
{
	options.objectProperties = new Point3DProperties(DEBUG_RENDER_POINT3D, position, size, duration, texture);

	worldRenderObjects.push_back(options);
}
 
