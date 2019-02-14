//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/SpriteAnimDefenition.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include <vector>
#include <map>

//Forward Declarations
class ShaderStage;
class Shader;
class ColorTargetView;
class RenderBuffer;
class UniformBuffer;
class VertexBuffer;
class TextureView2D;
class Texture;
class Texture2D;
class TextureView;
class BitmapFont;

//D3D11
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;

//------------------------------------------------------------------------------------------------------------------------------
class RenderContext
{
	friend class ShaderStage;
	friend class Shader;
	friend class RenderBuffer;
	friend class UniformBuffer;
	friend class Sampler;
	friend class Texture2D;

public:
	explicit RenderContext(void* windowHandle);
	~RenderContext();

	void				Startup();
	void				PremakeDefaults();
	void				BeginFrame();
	ColorTargetView*	GetFrameColorTarget();
	void				EndFrame();

	void				BeginCamera( Camera& camera );
	void				EndCamera();

	void				Shutdown();

	//Get resources
	//Texture*			CreateOrGetTextureFromFile(const char* imageFilePath);
	TextureView*		GetOrCreateTextureViewFromFile( std::string const &filename, bool isFont = false); 
	BitmapFont*			CreateOrGetBitmapFontFromFile (const std::string& bitmapName);
	Shader*				CreateOrGetShaderFromFile(const std::string& fileName);

	void				BindShader(Shader* shader);
	void				SetBlendMode(eBlendMode blendMode);
	
	//void				BindTexture(Texture* texture);
	void				BindTextureView( uint slot, TextureView *view ); 
	void				BindSampler( uint slot, Sampler *sampler ); 
	void				BindTextureView( uint slot, std::string const &name ); 
	void				BindSampler( eSampleMode mode );

	// more convince - I store samplers WITH a texture view for convenience (what
	// is the desired way to sample this texture).  
	// (Personal Note: I like storing the Sampler on the shader, as the sampling usually has to do with the 
	// effect I want to achieve, but that seems to be a fairly uncommon practice)

	// sampler I'm storing with the view in this design - but still giving
	// the context the option of binding a view with a different sampler if we so choose; 
	void				BindTextureViewWithSampler( uint slot, TextureView *view ); 
	void				BindTextureViewWithSampler( uint slot, std::string const &name ); 
	void				BindTextureViewWithSampler( uint slot, TextureView *view, Sampler *sampler ); 
	void				BindTextureViewWithSampler( uint slot, TextureView *view, eSampleMode mode ); 
	void				BindTextureViewWithSampler( uint slot, std::string const &name, eSampleMode mode ); 


	void				Draw(uint vertexCount, uint byteOffset = 0U);

	void				ClearColorTargets( const Rgba& clearColor );		//Previously was ClearScreen

	// Stream Data
	void				BindVertexStream( VertexBuffer *vbo ); 

	// Uniform/Constant Data
	void				UpdateFrameBuffer();
	void				BindUniformBuffer( uint slot, UniformBuffer *ubo ); 

	// Resurrected Functions
	void				DrawVertexArray( Vertex_PCU const *vertices, uint count ); 


	void				DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void				DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);

	
public:
	//White texture
	unsigned char		plainWhiteImageData[4] = {255, 255, 255, 255};

private:
	//D3D11 Functions
	bool				D3D11Setup(void* hwnd);   // Creates required D3D Objects
	void				D3D11Cleanup();          // Cleans up D3D11 Objects

	void				DemoRender();            // Does rendering for this demo

	// Private (internal) member functions will go here
	TextureView*		CreateTextureViewFromFile( const char* imageFilePath);
	BitmapFont*			CreateBitmapFontFromFile(const std::string& bitmapName);
	Shader*				CreateShaderFromFile(const std::string& fileName);
	void				CreateAndSetDefaultRasterState();

private:
	// Private (internal) data members will go here
	std::map< std::string, TextureView* >				m_loadedTextures;
	std::map< std::string, BitmapFont* >				m_loadedFonts;
	std::map< std::string, Shader*>						m_loadedShaders;

	ID3D11Device										*m_D3DDevice = nullptr;
	ID3D11DeviceContext									*m_D3DContext = nullptr;
	IDXGISwapChain										*m_D3DSwapChain = nullptr;
	ID3D11RasterizerState								*m_defaultRasterState = nullptr; 

	//For now make a global render target view
	//Wait no! we shouldn't be fucking with this
	//ID3D11RenderTargetView							*m_renderTargetView = nullptr;

	ColorTargetView*									m_FrameBuffer_ColorTargetView = nullptr;
	Camera*												m_currentCamera = nullptr;
	Shader*												m_currentShader = nullptr;

	void*												m_hwnd = nullptr;

public:

	VertexBuffer*										m_immediateVBO = nullptr; 
	UniformBuffer*										m_immediateUBO = nullptr;

	// There are a small set of sampler state that
	// we can get by with just reusing everywhere, 
	// so instead of creating a sampler per texture, 
	// we'll pull from this store; 
	Sampler*											m_cachedSamplers[SAMPLE_MODE_COUNT]; 
	std::map<std::string, TextureView*>					m_cachedTextureViews;

	unsigned int										m_frameCount = 0;
};


