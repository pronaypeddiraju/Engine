#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"

class Texture
{
	friend class RenderContext;
public: 
	unsigned int			GetTextureID() const	{return m_textureID; }
	
protected:
	//Only render context can set my m_textureID so we don't need a method for it since RenderContext is my friend :')
	//void					SetTextureID(unsigned int TextureID)	{m_textureID = TextureID;}

private:
	unsigned int			m_textureID = 0;
	IntVec2					m_dimensions;

};

