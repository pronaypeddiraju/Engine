//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/IsoSpriteDefenition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Renderer/TextureView.hpp"

//------------------------------------------------------------------------------------------------------------------------------
IsoSpriteDefenition::IsoSpriteDefenition(SpriteDefenition* spriteDefenitions, const Vec3 directions[], uint numDefenitions)
{
	//Populate all the spriteDefenitions into vector m_sprites
	for (uint defIndex = 0; defIndex < numDefenitions; ++defIndex)
	{
		m_sprites.push_back(&spriteDefenitions[defIndex]);
		m_directions.push_back(directions[defIndex]);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
IsoSpriteDefenition::~IsoSpriteDefenition()
{

}

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition* IsoSpriteDefenition::GetSpriteForLocalDirection(const Vec3& direction)
{
	uint bestIndex = 0;
	float bestValue = GetDotProduct(direction, m_directions[0]);

	for (uint i = 1; i < m_directions.size(); ++i) 
	{
		float value = GetDotProduct(direction, m_directions[i]);
		if(value > bestValue) 
		{
			//More accurate representation of the direction we received
			bestIndex = i;
			bestValue = value;
		}
	}

	//Return the sprite corresponding to the direction facing we received
	return m_sprites[bestIndex];
}
