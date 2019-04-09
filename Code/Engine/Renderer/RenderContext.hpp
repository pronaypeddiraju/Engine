//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include <vector>
#include <map>

//------------------------------------------------------------------------------------------------------------------------------
//Forward Declarations
class BitmapFont;
class ColorTargetView;
class GPUMesh;
class Image;
class IndexBuffer;
class Material;
class RenderBuffer;
class Shader;
class ShaderStage;
class Texture;
class Texture2D;
class TextureView;
class TextureView2D;
class UniformBuffer;
class VertexBuffer;
class WindowContext;

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
	friend class Texture;
	friend class DepthStencilTargetView;

public:
	explicit RenderContext(WindowContext* window);
	~RenderContext();

	void						Startup();
	void						PremakeDefaults();
	void						BeginFrame();

	ColorTargetView*			GetFrameColorTarget();
	DepthStencilTargetView*		GetFrameDepthStencilTarget(); 
	void						SetRasterStateWireFrame();
	void						CreateAndSetDefaultRasterState();


	void						EndFrame();

	void						BeginCamera( Camera& camera );
	void						EndCamera();

	void						Shutdown();

	//Lighting
	void						SetAmbientLight( Rgba const &color, float intensity ); 
	void						EnableLight( uint slot, LightT const &info );         
	void						DisableLight( uint slot );                           
	void						UpdateLightBuffer();

	void						EnableDirectionalLight();
	void						DisableDirectionalLight();

	//Get resources
	TextureView*				GetOrCreateTextureViewFromFile( std::string const &filename, bool isFont = false ); 
	BitmapFont*					CreateOrGetBitmapFontFromFile( const std::string& bitmapName );
	Shader*						CreateOrGetShaderFromFile( const std::string& fileName );
	Material*					CreateOrGetMaterialFromFile( const std::string& fileName );

	//Shader data
	void						BindShader(Shader* shader);
	void						SetBlendMode(eBlendMode blendMode);
	void						SetDepth(bool write);
	
	//Material data
	void						BindMaterial(Material* material);
	Sampler*					GetSamplerOfType( const std::string& m_samplerType );

	//Texture data
	void						BindTextureView( uint slot, TextureView *view ); 
	void						BindSampler( uint slot, Sampler *sampler ); 
	void						BindTextureView( uint slot, std::string const &name ); 
	void						BindSampler( eSampleMode mode );

	void						BindTextureViewWithSampler( uint slot, TextureView *view ); 
	void						BindTextureViewWithSampler( uint slot, std::string const &name ); 
	void						BindTextureViewWithSampler( uint slot, TextureView *view, Sampler *sampler ); 
	void						BindTextureViewWithSampler( uint slot, TextureView *view, eSampleMode mode ); 
	void						BindTextureViewWithSampler( uint slot, std::string const &name, eSampleMode mode ); 

	// Be able to set a model matrix (updates the uniform buffer; 
	void						SetGlobalTint(const Rgba& color);
	void						SetModelMatrix( Matrix44 const &modelMatrix );            // A04
	inline const Matrix44&		GetProjectionMatrix()	{return m_currentCamera->m_projection;}
	inline const Matrix44&		GetViewMatrix() {return m_currentCamera->m_view;}

	void						ClearColorTargets( const Rgba& clearColor );		//Previously was ClearScreen
	void						ClearDepthStencilTarget( float depth = 1.0f, uint8_t stencil = 0U ); // A04

	// Stream Data
	void						BindVertexStream( VertexBuffer *vbo ); 
	// Be able to bind index buffers; 
	void						BindIndexStream( IndexBuffer *ibo );    // A04

	// Uniform/Constant Data
	void						BindModelMatrix( Matrix44 const &model );
	void						UpdateFrameBuffer();
	void						BindUniformBuffer( uint slot, UniformBuffer *ubo ); 

	//Draw Calls
	bool						PreDraw( GPUMesh *mesh );
	
	void						Draw(uint vertexCount, uint byteOffset = 0U);
	void						DrawIndexed( uint indexCount);                                 // A04

	void						DrawVertexArray( Vertex_PCU const *vertices, uint count ); 
	void						DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void						DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);

	void						DrawMesh( GPUMesh *mesh );                                         // A04
	
public:
	//White texture
	unsigned char				plainWhiteImageData[4] = {255, 255, 255, 255};

private:
	//D3D11 Functions
	bool						D3D11Setup(void* hwnd);   // Creates required D3D Objects
	void						D3D11Cleanup();          // Cleans up D3D11 Objects

	void						DemoRender();            // Does rendering for this demo

	// Private (internal) member functions will go here
	BitmapFont*					CreateBitmapFontFromFile(const std::string& bitmapName);
	Shader*						CreateShaderFromFile(const std::string& fileName);

private:
	// Private (internal) data members will go here
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

	//Matrix44											m_modelMatrix;

public:

	VertexBuffer*										m_immediateVBO = nullptr; 
	UniformBuffer*										m_immediateUBO = nullptr;
	Image*												m_whiteImage = nullptr;

	Texture2D*											m_defaultDepthTexture = nullptr;
	DepthStencilTargetView*								m_defaultDepthStencilView = nullptr; // A04
	UniformBuffer*										m_modelBuffer = nullptr;                      // A04

	// A06 - constant buffer storing lights; 
	UniformBuffer*										m_gpuLightBuffer = nullptr;
	// A06 - CPU copy of light data
	LightBufferT										m_cpuLightBuffer;                   
	// A06 - need to update the light buffer before a draw
	bool												m_lightBufferDirty = true;                           
	uint												m_lightSlot = 0U;

	Sampler*											m_cachedSamplers[SAMPLE_MODE_COUNT]; 
	std::map<std::string, TextureView*>					m_cachedTextureViews;
	std::map<std::string, Material*>					m_materialDatabase;

	unsigned int										m_frameCount = 0;

	GPUMesh*											m_immediateMesh = nullptr;   //A04
	ModelBufferT										m_cpuModelBuffer;
};


