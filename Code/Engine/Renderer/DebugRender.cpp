//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <math.h>
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

void DebugRender::Shutdown()
{

}

void DebugRender::Set2DCamera( Camera* camera )
{
	m_debug2DCam = camera;
}

Camera& DebugRender::Get2DCamera()
{
	return *m_debug2DCam;
}

void DebugRender::DebugRenderPoint2D( DebugRenderOptionsT const &options, Vec2 position, float size /*= DEFAULT_POINT_SIZE */ )
{
	m_debug2DCam->SetModelMatrix(Matrix44::IDENTITY);
	m_debug2DCam->UpdateUniformBuffer(m_renderContext);

	m_renderContext->BindTextureViewWithSampler(0U, nullptr);

	if(options.mode == DEBUG_RENDER_USE_DEPTH)
	{
		m_renderContext->SetDepth(true);
	}

	//TODO: Implement code to handle duration logic
	Vec2 minBounds = Vec2(position.x * size * -0.5f, position.y * size * -0.5f);
	Vec2 maxBounds = Vec2(position.x * size * 0.5f, position.y * size * 0.5f);
	AABB2 box = AABB2(minBounds, maxBounds);
	std::vector<Vertex_PCU> pointVerts;
	AddVertsForAABB2D(pointVerts, box, options.endColor );
	m_renderContext->DrawVertexArray(pointVerts);

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
