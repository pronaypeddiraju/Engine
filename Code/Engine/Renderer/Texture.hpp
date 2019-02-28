#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/RendererTypes.hpp"

struct ID3D11Resource;

class Image;
class TextureView2D;
class DepthStencilTargetView;
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
class Texture
{
	friend class RenderContext;
public: 
	Texture( RenderContext *renderContext ); 
	virtual ~Texture(); // virtual, as this will release the resource; 

	unsigned int			GetTextureID() const	{return m_textureID; }
	void					FreeHandles();


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
	Texture2D( RenderContext *renderContext );  // textures always come from a context; 
	virtual ~Texture2D(); 

	bool				LoadTextureFromFile( std::string const &filename, bool isFont = false );
	bool				LoadTextureFromImage( Image const &image ); 

	bool				CreateDepthStencilTarget( uint width, uint height ); 

	// Create a view of this texture usable in the shader; 
	TextureView2D*				CreateTextureView2D() const;  
	DepthStencilTargetView*		CreateDepthStencilTargetView();  // A04

	static Texture2D* CreateDepthStencilTarget( RenderContext *renderContext, uint width, uint height );   // A04
	static Texture2D* CreateDepthStencilTargetFor( Texture2D *colorTarget );   // A04
};
