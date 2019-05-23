//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"
#include <vector>

class TextureView;
struct IntVec2;

//------------------------------------------------------------------------------------------------------------------------------
class SpriteSheet
{
public: 
	
	explicit SpriteSheet(TextureView* texture, const IntVec2 spriteGridDefenition);

	//Accessors
	const SpriteDefenition&				GetSpriteDef(int spriteIndex) const;
	const SpriteDefenition&				GetSpriteDef( IntVec2 spriteCoords, int sheetWidth ) const;

protected:
	TextureView*						m_spriteTexture = nullptr;
	std::vector<SpriteDefenition>		m_spriteDefs;
};