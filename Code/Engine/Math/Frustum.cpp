//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Frustum.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
bool Frustum::ContainsPoint(const Vec3& pos) const
{
	//If pos has +ve distance from all faces it is outside the frustum
	TODO("Test Frustum Contains point logic");

	bool result = true;

	for (int i = 0; i < 6; i++)
	{
		if (GetDotProduct(m_planes[i].m_normal, pos) < 0.f)
		{
			result = false;
		}
	}

	return result;
}

