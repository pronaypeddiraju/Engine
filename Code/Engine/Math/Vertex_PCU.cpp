//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/BufferLayout.hpp" 
#include "Engine/Math/VertexMaster.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords )
{
	m_position = position;
	m_color = color;
	m_uvTexCoords = uvTexCoords;
}

Vertex_PCU::Vertex_PCU( const VertexMaster& master )
{
	m_position = master.m_position;
	m_color = master.m_color;
	m_uvTexCoords = master.m_uv;
}

STATIC BufferAttributeT Vertex_PCU::LAYOUT[] = {
	BufferAttributeT( "POSITION",  DF_VEC3,      offsetof(Vertex_PCU, m_position) ), 
	BufferAttributeT( "COLOR",     DF_RGBA32,    offsetof(Vertex_PCU, m_color) ), 
	BufferAttributeT( "UV",        DF_VEC2,      offsetof(Vertex_PCU, m_uvTexCoords) ), 
	BufferAttributeT() // end		
};


STATIC void Vertex_PCU::CopyFromMaster( void *buffer, VertexMaster const *src, uint count )
{
	Vertex_PCU *dst = (Vertex_PCU*)buffer; 

	for (uint i = 0; i < count; ++i) 
	{
		dst[i].m_position = src[i].m_position; 
		dst[i].m_color = src[i].m_color;
		dst[i].m_uvTexCoords = src[i].m_uv;
	}
}

