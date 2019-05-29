//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Frustum.hpp"

//------------------------------------------------------------------------------------------------------------------------------
bool Frustum::ContainsPoint(const Vec3& pos) const
{
	//If pos has +ve distance from all faces it is outside the frustum
	return false;
}

