//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/IsoSpriteDefenition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/TextureView.hpp"

//------------------------------------------------------------------------------------------------------------------------------
IsoSpriteDefenition::IsoSpriteDefenition(const SpriteDefenition spriteDefenitions[], uint numDefenitions)
{
	m_directions.clear();

	m_directions.push_back(Vec3(0.f, 0.f, -1.f));
	m_directions.push_back(Vec3(-1.f, 0.f, -1.f).GetNormalized());
	m_directions.push_back(Vec3(-1.f, 0.f, 0.f));
	m_directions.push_back(Vec3(-1.f, 0.f, 1.f).GetNormalized());
	m_directions.push_back(Vec3(0.f, 0.f, 1.f));
	m_directions.push_back(Vec3(1.f, 0.f, 1.f).GetNormalized());
	m_directions.push_back(Vec3(1.f, 0.f, 0.f));
	m_directions.push_back(Vec3(1.f, 0.f, -1.f).GetNormalized());

	//Populate all the spriteDefenitions into vector m_sprites
	for (uint defIndex = 0; defIndex < numDefenitions; ++defIndex)
	{
		SpriteDefenition* defenition = new SpriteDefenition(spriteDefenitions[defIndex]);
		m_sprites.push_back(defenition);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
IsoSpriteDefenition::IsoSpriteDefenition()
{

}

//------------------------------------------------------------------------------------------------------------------------------
IsoSpriteDefenition::~IsoSpriteDefenition()
{

}

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition& IsoSpriteDefenition::GetSpriteForLocalDirection(const Vec3& direction) const
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
	return *m_sprites[bestIndex];
}
