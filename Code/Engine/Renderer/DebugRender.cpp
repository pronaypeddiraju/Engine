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
			objectProperties->m_mesh->SetColor(objectProperties->m_currentColor);
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
		case DEBUG_RENDER_DISC:			{	DrawDisc2D(renderObject);		}	break;
		case DEBUG_RENDER_RING:			{	DrawRing2D(renderObject);		}	break;
		case DEBUG_RENDER_WIRE_QUAD:	{	DrawWireQuad2D(renderObject);	}	break;
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
		case DEBUG_RENDER_LINE3D:		{	DrawLine3D(renderObject);		}	break;
		case DEBUG_RENDER_SPHERE:		{	DrawSphere(renderObject);		}	break;
		case DEBUG_RENDER_BOX:			{	DrawBox(renderObject);			}	break;
		case DEBUG_RENDER_QUAD3D:		{	DrawQuad3D(renderObject);		}	break;
		default:						{	ERROR_AND_DIE("The debug object is not yet defined in DebugRenderToCamera");	}	break;
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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(boxVerts);
}

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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(boxVerts);
}

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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(ringVerts);
}


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
	case DEBUG_RENDER_XRAY:
	//Make 2 draw calls here
	//One with compare op lequals and one with compare op greater than (edit alpha on that one)
	break;
	}

	m_renderContext->DrawVertexArray(ringVerts);
}

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
	point.CreateFromCPUMesh( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForBillBoard(objectProperties->m_position);

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

	m_renderContext->DrawMesh(&point);

}

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
	quad.CreateFromCPUMesh( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForBillBoard(objectProperties->m_position);

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

	m_renderContext->DrawMesh(&quad);
}

void DebugRender::DrawLine3D( const DebugRenderOptionsT* renderObject ) const
{
	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	Line3DProperties* objectProperties = reinterpret_cast<Line3DProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_LINE3D)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a 3D Line. Check inputs");
	}

	/*
	std::vector<Vertex_PCU> lineVerts;
	Vec2 boxMins = Vec2((objectProperties->m_startPos.x - objectProperties->m_lineWidth * 0.5f), (objectProperties->m_startPos.y - objectProperties->m_lineWidth * 0.5f));
	Vec2 boxMaxs = Vec2((objectProperties->m_endPos.x + objectProperties->m_lineWidth * 0.5f), (objectProperties->m_endPos.y + objectProperties->m_lineWidth * 0.5f));
	AABB2 lineBox = AABB2(boxMins, boxMaxs);


	SetObjectMatrixForPosition(objectProperties->m_center);
	AddVertsForAABB2D(lineVerts, lineBox, objectProperties->m_currentColor);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, nullptr); 
	*/

	//Setup mesh here
	GPUMesh line = GPUMesh( m_renderContext ); 
	line.CreateFromCPUMesh( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_center);
	//SetObjectMatrixForBillBoard(objectProperties->m_center);

	//Setup the textures on the render context
	m_renderContext->BindTextureViewWithSampler(0U, nullptr); 

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

	m_renderContext->DrawMesh(&line);
}

void DebugRender::DrawSphere( const DebugRenderOptionsT* renderObject ) const
{
	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	SphereProperties* objectProperties = reinterpret_cast<SphereProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_SPHERE)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Sphere. Check inputs");
	}

	//Setup mesh here
	GPUMesh sphere = GPUMesh( m_renderContext ); 
	sphere.CreateFromCPUMesh( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_center);

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

	m_renderContext->DrawMesh(&sphere);
}

void DebugRender::DrawBox( const DebugRenderOptionsT* renderObject ) const
{
	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	BoxProperties* objectProperties = reinterpret_cast<BoxProperties*>(renderObject->objectProperties);
	if(objectProperties->m_renderObjectType != DEBUG_RENDER_BOX)
	{
		ERROR_AND_DIE("Object recieved in DebugRender was not a Box. Check inputs");
	}

	//Setup mesh here
	GPUMesh box = GPUMesh( m_renderContext ); 
	box.CreateFromCPUMesh( objectProperties->m_mesh, GPU_MEMORY_USAGE_STATIC );
	SetObjectMatrixForPosition(objectProperties->m_position);

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

	m_renderContext->DrawMesh(&box);
}

void DebugRender::SetObjectMatrixForPosition( Vec3 position ) const
{
	//Setup matrix for position
	Matrix44 objectMatrix = Matrix44::MakeFromEuler(Vec3::ZERO);
	Matrix44::SetTranslation3D(position, objectMatrix);

	//Set on Render Context
	m_renderContext->SetModelMatrix( objectMatrix ); 
}

void DebugRender::SetObjectMatrixForBillBoard( Vec3 position ) const
{
	Matrix44 cameraModel = m_debug3DCam->GetModelMatrix();
	Matrix44 objectModel = Matrix44::IDENTITY;
	objectModel.SetRotationFromMatrix(objectModel, cameraModel);

	objectModel = Matrix44::SetTranslation3D(position, objectModel);

	m_renderContext->SetModelMatrix(objectModel);	
}

void DebugRender::DebugRenderPoint2D( DebugRenderOptionsT options, const Vec2& position, float duration, float size /*= DEFAULT_POINT_SIZE */ )
{
	options.objectProperties = new Point2DProperties(DEBUG_RENDER_POINT, position, duration, size);

	screenRenderObjects.push_back(options);
}

//Inplementation to debug render a line using 2D debug camera
void DebugRender::DebugRenderLine2D(DebugRenderOptionsT options, const Vec2& start, const Vec2& end, float duration, float lineWidth)
{
	options.objectProperties = new Line2DProperties(DEBUG_RENDER_LINE, start, end, duration, lineWidth);

	screenRenderObjects.push_back(options);

}

void DebugRender::DebugRenderQuad2D( DebugRenderOptionsT options, AABB2 const & quad, float duration, TextureView* texture )
{
	options.objectProperties = new Quad2DProperties(DEBUG_RENDER_QUAD, quad, duration, DEFAULT_WIRE_WIDTH_2D, texture);

	screenRenderObjects.push_back(options);
}

void DebugRender::DebugRenderWireQuad2D( DebugRenderOptionsT options, AABB2 const &quad, float duration /*= 0.f*/, float thickness /*= DEFAULT_WIRE_WIDTH_2D */ )
{
	options.objectProperties = new Quad2DProperties(DEBUG_RENDER_WIRE_QUAD, quad, duration, thickness);

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

void DebugRender::DebugRenderDisc2D( DebugRenderOptionsT options, Disc2D const &disc, float duration /*= 0.f*/)
{
	options.objectProperties = new Disc2DProperties(DEBUG_RENDER_DISC, disc, 0.f, duration);

	screenRenderObjects.push_back(options);
}

void DebugRender::DebugRenderRing2D( DebugRenderOptionsT options, Disc2D const &disc, float duration /*= 0.f*/, float thickness /*= DEFAULT_DISC_THICKNESS */ )
{
	options.objectProperties = new Disc2DProperties(DEBUG_RENDER_RING, disc, thickness, duration);

	screenRenderObjects.push_back(options);
}

void DebugRender::DebugRenderPoint( DebugRenderOptionsT options, const Vec3& position, float duration, float size, TextureView* texture )
{
	options.objectProperties = new Point3DProperties(DEBUG_RENDER_POINT3D, position, size, duration, texture);

	worldRenderObjects.push_back(options);
}
 
void DebugRender::DebugRenderLine( DebugRenderOptionsT options, const Vec3& start, const Vec3& end, float duration, float lineWidth /*= DEFAULT_LINE_WIDTH */ )
{
	options.objectProperties = new Line3DProperties(DEBUG_RENDER_LINE3D, start, end, duration, lineWidth);

	worldRenderObjects.push_back(options);
}

void DebugRender::DebugRenderSphere( DebugRenderOptionsT options, Vec3 center, float radius, float duration /*= 0.f*/, TextureView* texture /*= nullptr */ )
{
	options.objectProperties = new SphereProperties(DEBUG_RENDER_SPHERE, center, radius, duration, texture);

	worldRenderObjects.push_back(options);
}

void DebugRender::DebugRenderBox( DebugRenderOptionsT options, const AABB3& box, const Vec3& position, float duration /*= 0.f*/, TextureView* texture /*= nullptr */ )
{
	options.objectProperties = new BoxProperties(DEBUG_RENDER_BOX, box, position, duration, texture);

	worldRenderObjects.push_back(options);
}

void DebugRender::DebugRenderQuad( DebugRenderOptionsT options, const AABB2& quad, const Vec3& position, float duration /*= 0.f*/, TextureView* texture /*= nullptr*/ )
{
	options.objectProperties = new Quad3DProperties(DEBUG_RENDER_QUAD3D, quad, position, duration, texture);

	worldRenderObjects.push_back(options);
}
