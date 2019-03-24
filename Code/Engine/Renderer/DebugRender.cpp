//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
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

	delete m_debug3DCam;
	m_debug3DCam = nullptr;
}

void DebugRender::Update( float deltaTime )
{
	UNUSED(deltaTime);
}

void DebugRender::DebugRenderToScreen() const
{
	//Use this method to render to screen camera
	int vectorSize = static_cast<int>(screenRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		const DebugRenderOptionsT* renderObject = &screenRenderObjects[objectIndex];
		switch(renderObject->objectProperties->m_renderObjectType)
		{
		case DEBUG_RENDER_POINT:
		{
			DrawPoint2D( renderObject);
		}
		break;
		case DEBUG_RENDER_LINE:
		{
			DrawLine2D(renderObject);
		}
		break;
		default:
			ERROR_AND_DIE("The debug object is not yet defined in DebugRenderToScreen");
			break;
		}
	}
}

void DebugRender::DebugRenderToCamera( Camera* renderCamera ) const
{
	UNUSED(renderCamera);

	//Use this method to render to the world camera
	int vectorSize = static_cast<int>(worldRenderObjects.size());

	for(int objectIndex = 0; objectIndex < vectorSize; objectIndex++)
	{
		const DebugRenderOptionsT* renderObject = &worldRenderObjects[objectIndex];
		switch(renderObject->objectProperties->m_renderObjectType)
		{
		case DEBUG_RENDER_POINT3D:
		//DrawPoint2D(renderContext, renderObject);
		DrawPoint3D(renderObject);
		break;
		default:
		GUARANTEE_OR_DIE(true, "The debug object is not yet defined in DebugRenderToScreen");
		break;
		}
	}
}

Camera& DebugRender::Get2DCamera()
{
	return *m_debug2DCam;
}

void DebugRender::DrawPoint2D( const DebugRenderOptionsT* renderObject) const
{
	m_debug2DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug2DCam->UpdateUniformBuffer(m_renderContext);

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Point2DObjectProperties* objectProperties = reinterpret_cast<Point2DObjectProperties*>(renderObject->objectProperties);
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
	AddVertsForAABB2D(pointVerts, box, renderObject->beginColor );

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

void DebugRender::DrawLine2D( const DebugRenderOptionsT* renderObject ) const
{
	m_debug2DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug2DCam->UpdateUniformBuffer(m_renderContext);

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Line2DObjectProperties* objectProperties = reinterpret_cast<Line2DObjectProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_LINE)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Line. Check inputs");
	}

	//TODO: Implement code to handle duration logic
	std::vector<Vertex_PCU> lineVerts;
	AddVertsForLine2D(lineVerts, objectProperties->m_startPos, objectProperties->m_endPos, objectProperties->m_lineWidth, renderObject->beginColor);

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

void DebugRender::DrawPoint3D( const DebugRenderOptionsT* renderObject ) const
{
	UNUSED(renderObject);
	//Create the Debug Render object here

}

void DebugRender::DebugRenderPoint2D( DebugRenderOptionsT options, Vec2 position, float duration, float size /*= DEFAULT_POINT_SIZE */ )
{
	options.objectProperties = new Point2DObjectProperties(DEBUG_RENDER_POINT, position, duration, size);

	screenRenderObjects.push_back(options);
}

//Inplementation to debug render a line using 2D debug camera
void DebugRender::DebugRenderLine2D(DebugRenderOptionsT options, Vec2 start, Vec2 end, float duration, float lineWidth)
{
	options.objectProperties = new Line2DObjectProperties(DEBUG_RENDER_LINE, start, end, duration, lineWidth);

	screenRenderObjects.push_back(options);

}

void DebugRender::DebugRenderQuad2D( DebugRenderOptionsT const & options, AABB2 const & quad )
{
	m_debug2DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug2DCam->UpdateUniformBuffer(m_renderContext);

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	std::vector<Vertex_PCU> boxVerts;
	AddVertsForAABB2D(boxVerts, quad, options.beginColor);

	//TODO: Implement code to handle duration logic
	switch (options.mode)
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

void DebugRender::DebugRenderPoint( DebugRenderOptionsT const& options, const Vec3& position, const Rgba& color, float size )
{
	//Create a quad in 3D space and make it face the camera

	//m_debug3DCam

	m_debug3DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug3DCam->UpdateUniformBuffer(m_renderContext);

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	std::vector<Vertex_PCU> pointVerts;
	Vec2 boxMins = Vec2((position.x - size * 0.5f), (position.y - size * 0.5f));
	Vec2 boxMaxs = Vec2((position.x + size * 0.5f), (position.y + size * 0.5f));
	AABB2 pointBox = AABB2(boxMins, boxMaxs);



	AddVertsForAABB2D(pointVerts, pointBox, color);
}

void DebugRender::DebugRenderPoint( Camera & camera, DebugRenderOptionsT const & options, Vec3 position, TextureView* texture, float size )
{
	m_debug3DCam = &camera;

	m_debug3DCam->UpdateUniformBuffer(m_renderContext);

	//Setup the point's model matrix
	Matrix44 pointTransform = m_debug3DCam->GetModelMatrix();
	Vec3 euler = camera.GetEuler();
	Matrix44 objectMatrix = Matrix44::MakeFromEuler(euler);
	Matrix44::SetTranslation3D(position, objectMatrix);

	//objectMatrix.AppendMatrix(pointTransform);
	//Setup the matrix and textures on the render context used
	m_renderContext->BindTextureViewWithSampler(0U, texture); 
	m_renderContext->SetModelMatrix( objectMatrix ); 
	
	//Draw
	std::vector<Vertex_PCU> pointVerts;
	Vec2 boxMins = Vec2((-size * 0.5f), (-size * 0.5f));
	Vec2 boxMaxs = Vec2((size * 0.5f), (size * 0.5f));
	AABB2 pointBox = AABB2(boxMins, boxMaxs);

	AddVertsForAABB2D(pointVerts, pointBox, Rgba::DARK_GREY);

	//TODO: Implement code to handle duration logic
	switch (options.mode)
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
	m_debug3DCam = nullptr;
}
