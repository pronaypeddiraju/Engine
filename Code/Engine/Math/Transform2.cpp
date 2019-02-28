//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Transform2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Transform2::Transform2( const Vec2& position, float rotation, const Vec2& scale )
{
	m_position = position;
	m_rotation = rotation;
	m_scale = scale;
}

Transform2::Transform2( const Vec2& position )
{
	m_position = position;
	m_rotation = 0.f;
	m_scale = Vec2::ONE;
}

Transform2::Transform2( const Vec2& position, float rotation )
{
	m_position = position;
	m_rotation = rotation;
}

Transform2::Transform2( const Vec2& position, const Vec2& scale )
{
	m_position = position;
	m_scale = scale;
}

Transform2::Transform2()
{

}

Transform2::~Transform2()
{

}

