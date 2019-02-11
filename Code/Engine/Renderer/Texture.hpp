#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

struct ID3D11Resource;

class Image;
class TextureView2D;

// Eventually, when we add new Texture types, these common 
// enums & classes should be moved to Texture.hpp
enum eTextureUsageBit : uint
{
	TEXTURE_USAGE_TEXTURE_BIT              = BIT_FLAG(0),    // Can be used to create a TextureView
	TEXTURE_USAGE_COLOR_TARGET_BIT         = BIT_FLAG(1),    // Can be used to create a ColorTargetView
	TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT = BIT_FLAG(2),    // Can be used to create a DepthStencilTargetView
}; 
typedef uint eTextureUsageBits; 

class Texture
{
	friend class RenderContext;
public: 
	Texture( RenderContext *renderContext ); 
	virtual ~Texture(); // virtual, as this will release the resource; 

	unsigned int			GetTextureID() const	{return m_textureID; }
	
protected:
	//Only render context can set my m_textureID so we don't need a method for it since RenderContext is my friend :')
	//void					SetTextureID(unsigned int TextureID)	{m_textureID = TextureID;}

private:
	unsigned int			m_textureID = 0;

public:
	RenderContext*			m_owner = nullptr; 
	IntVec2					m_dimensions = IntVec2::ZERO;
	// D3D11 objets; 
	ID3D11Resource*			m_handle = nullptr; 
	eGPUMemoryUsage			m_memoryUsage; 
	eTextureUsageBits		m_textureUsage; 

	// If you have the concept of an image format (RGBA8, R8, D24S8, etc..)
	// It would go here.  If you only have support for RGBA8, we'll just assume it; 
	// eImageFormat m_format; 
};

//------------------------------------------------------------------------
class Texture2D : public Texture
{
public:
	Texture2D( RenderContext *ctx );  // texures always come from a context; 
	virtual ~Texture2D(); 

	bool LoadTextureFromFile( std::string const &filename );
	bool LoadTextureFromImage( Image const &image ); 

	// Create a view of this texture usable in the shader; 
	TextureView2D* CreateTextureView2D() const;  
};
