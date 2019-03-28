//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
struct VertexMaster;
struct BufferAttributeT;
class BufferLayout;

typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
struct Vertex_Lit
{
public:
	Vec3 m_position;
	Vec3 m_normal; 
	Rgba m_color; 
	Vec2 m_uv; 

public:
	explicit Vertex_Lit(const VertexMaster& master);
	~Vertex_Lit() {}

	static BufferAttributeT LAYOUT[]; 
	static const BufferLayout* layout;
	static void CopyFromMaster( void *buffer, VertexMaster const *src, uint count ); 
};