#pragma once

#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteAnimDefenition.hpp"
#include <vector>
#include <map>

class ShaderStage;
class Shader;
class ColorTargetView;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

enum BlendMode
{
	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE
};

class RenderContext
{
	friend class ShaderStage;

public:
	explicit RenderContext(void* windowHandle);
	~RenderContext();

	void				Startup();

	void				BeginFrame();
	ColorTargetView*	GetFrameColorTarget();
	void				EndFrame();

	void				BeginCamera( Camera& camera );
	void				EndCamera();

	void				Shutdown();

	//Get resources
	Texture*			CreateOrGetTextureFromFile( const char* imageFilePath);
	BitmapFont*			CreateOrGetBitmapFontFromFile ( std::string bitmapName);
	Shader*				CreateOrGetShaderFromFile( std::string const &fileName);

	void				BindShader(Shader* shader);
	void				SetBlendMode(BlendMode blendMode);
	void				BindTexture(Texture* texture);

	void				Draw(uint vertexCount, uint byteOffset = 0U);

	void				ClearColorTargets( const Rgba& clearColor );		//Previously was ClearScreen
	
	void				DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void				DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);

	
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
	Shader*				CreateShaderFromFile(std::string fileName);

private:
	// Private (internal) data members will go here
	std::map< std::string, Texture* >					m_loadedTextures;
	std::map< std::string, BitmapFont* >				m_loadedFonts;
	std::map< std::string, Shader*>						m_loadedShaders;

	ID3D11Device										*m_D3DDevice = nullptr;
	ID3D11DeviceContext									*m_D3DContext = nullptr;
	IDXGISwapChain										*m_D3DSwapChain = nullptr;

	//For now make a global rtv
	//Wait no! we shouldnt be fucking with this
	//ID3D11RenderTargetView								*m_renderTargetView = nullptr;

	ColorTargetView*									m_FrameBuffer_ColorTargetView = nullptr;
	Camera*												m_currentCamera = nullptr;


};


