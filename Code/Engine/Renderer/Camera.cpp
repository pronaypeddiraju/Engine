#include "Camera.hpp"

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;

	printf("Executed SetOrthoView");
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_orthoBottomLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_orthoTopRight;
}

void Camera::Translate2D( const Vec2& translation2D )
{
	m_orthoBottomLeft += translation2D;
	m_orthoTopRight += translation2D;
}

