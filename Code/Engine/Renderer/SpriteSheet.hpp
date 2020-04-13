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
	SpriteSheet();
	explicit SpriteSheet(TextureView* texture, const IntVec2& spriteGridDefenition);

	//Setters
	void								SetSpriteDefs(const std::vector<SpriteDefenition>& spriteDefs);
	void								SetSpriteTextureView(TextureView* textureView);

	//Accessors
	const SpriteDefenition&				GetSpriteDef(int spriteIndex) const;
	const SpriteDefenition&				GetSpriteDef( const IntVec2& spriteCoords, int sheetWidth ) const;

protected:
	TextureView*						m_spriteTexture = nullptr;
	std::vector<SpriteDefenition>		m_spriteDefs;
};