#pragma once

#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteAnimDefenition.hpp"
#include <vector>
#include <map>

enum BlendMode
{
	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE
};

class RenderContext
{
public:
	explicit RenderContext(void* windowHandle);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void DebugRender();

	Texture*			CreateOrGetTextureFromFile( const char* imageFilePath);
	BitmapFont*			CreateOrGetBitmapFontFromFile ( std::string bitmapName);

	void				EndUVDraw();	
	void				BindTexture(Texture* texture);


	void				ClearScreen( const Rgba& clearColor );
	void				BeginCamera( const Camera& camera );
	void				EndCamera( const Camera& camera );
	void				DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void				DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);
	void				SetBlendMode(BlendMode blendMode);
	
public:
	//White texture
	unsigned char plainWhiteImageData[4] = {255, 255, 255, 255};

private:
	//D3D11 Functions
	bool D3D11Setup(void* hwnd);   // Creates required D3D Objects
	void D3D11Cleanup();          // Cleans up D3D11 Objects

	void DemoRender();            // Does rendering for this demo
	void DemoRun();               // Demo setup and update loop

	// Private (internal) member functions will go here
	Texture*			CreateTextureFromFile( const char* imageFilePath);
	BitmapFont*			CreateBitmapFontFromFile(std::string bitmapName);

private:
	// Private (internal) data members will go here
	std::map< std::string, Texture* > m_loadedTextures;
	std::map< std::string, BitmapFont* > m_loadedFonts;

};


