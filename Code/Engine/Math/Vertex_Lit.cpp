//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

Vertex_Lit::Vertex_Lit( const VertexMaster& master )
{
	m_position = master.m_position;
	m_normal = master.m_normal;
	m_tangent = master.m_tangent;
	m_biTangent = master.m_biTangent;

	m_color = master.m_color;
	m_uv = master.m_uv;
}

STATIC BufferAttributeT Vertex_Lit::LAYOUT[] = {
	BufferAttributeT( "POSITION",  DF_VEC3,      offsetof(Vertex_Lit, m_position) ), 
	BufferAttributeT( "NORMAL",    DF_VEC3,      offsetof(Vertex_Lit, m_normal)   ), 
	BufferAttributeT( "TANGENT",   DF_VEC3,		 offsetof(Vertex_Lit, m_tangent)  ),
	BufferAttributeT( "BITANGENT", DF_VEC3,		 offsetof(Vertex_Lit, m_biTangent)),
	BufferAttributeT( "COLOR",     DF_RGBA32,    offsetof(Vertex_Lit, m_color)    ), 
	BufferAttributeT( "TEXCOORD",  DF_VEC2,      offsetof(Vertex_Lit, m_uv)		  ), 
	BufferAttributeT() // end		
};

const BufferLayout* Vertex_Lit::layout = BufferLayout::For<Vertex_Lit>();

STATIC void Vertex_Lit::CopyFromMaster( void *buffer, VertexMaster const *src, uint count )
{
	Vertex_Lit *dst = (Vertex_Lit*)buffer; 

	for (uint i = 0; i < count; ++i) 
	{
		dst[i].m_position = src[i].m_position; 
		dst[i].m_normal = src[i].m_normal;
		dst[i].m_tangent = src[i].m_tangent;
		dst[i].m_biTangent = src[i].m_biTangent;
		dst[i].m_color = src[i].m_color;
		dst[i].m_uv = src[i].m_uv;
	}
}
