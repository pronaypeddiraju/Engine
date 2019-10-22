//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Sampler.hpp"
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
class Model;
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
struct IntVec2;

//D3D11
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;

//------------------------------------------------------------------------------------------------------------------------------
class RenderContext
{
	friend class DepthStencilTargetView;
	friend class ImGUISystem;
	friend class Material;
	friend class RenderBuffer;
	friend class Sampler;
	friend class Shader;
	friend class ShaderStage;
	friend class Texture;
	friend class Texture2D;
	friend class UniformBuffer;

public:
	explicit RenderContext(WindowContext* window);
	~RenderContext();

	void						BeginFrame();
	void						EndFrame();
	void						Shutdown();

	ColorTargetView*			GetFrameColorTarget();
	Texture2D*					GetFrameColorTexture();

	void						SetRasterStateWireFrame();
	void						CreateAndSetDefaultRasterState();

	void						BeginCamera( Camera& camera );
	void						EndCamera();

	//Getters for DX
	ID3D11DeviceContext*		GetDXContext() const;
	ID3D11Device*				GetDXDevice() const;

	//Lighting
	void						SetAmbientLight( Rgba const &color, float intensity ); 
	void						EnableLight( uint slot, LightT const &info );         
	void						DisableLight( uint slot );                           
	void						UpdateLightBuffer();
	void						EnableDirectionalLight();
	void						DisableDirectionalLight();
	void						EnablePointLight(uint slot, const Vec3& position, const Vec3& direction,
												 const Rgba& color = Rgba::WHITE, float intensity = 1.f,
												 const Vec3& diffuseAttenuation = Vec3(1.f, 0.f, 0.f),
												 const Vec3& specularAttenuation = Vec3(1.f, 0.f, 0.f)) const;
	void						EnableDirectionalLight(const Vec3& position, const Vec3& direction,
													   const Rgba& color = Rgba::WHITE, float intensity = 1.f,
													   const Vec3& diffuseAttenuation = Vec3(1.f, 0.f, 0.f),
													   const Vec3& specularAttenuation = Vec3(1.f, 0.f, 0.f)) const;


	//Get resources
	TextureView*				CreateOrGetTextureViewFromFile( std::string const &filename, bool isFont = false ); 
	void						RegisterTextureView(std::string const &fileName, TextureView const *view);
	BitmapFont*					CreateOrGetBitmapFontFromFile(const std::string& bitmapName);
	Shader*						CreateOrGetShaderFromFile( const std::string& fileName );
	Material*					CreateOrGetMaterialFromFile( const std::string& fileName );
	GPUMesh*					CreateOrGetMeshFromFile( const std::string& fileName );

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
	void						SetModelMatrix( Matrix44 const &modelMatrix );      
	inline const Matrix44&		GetProjectionMatrix()	{return m_currentCamera->m_projection;}
	inline const Matrix44&		GetViewMatrix() {return m_currentCamera->m_view;}

	void						ClearColorTargets( const Rgba& clearColor );		
	void						ClearDepthStencilTarget( float depth = 1.0f, uint8_t stencil = 0U ); 

	// Stream Data
	void						BindVertexStream( VertexBuffer *vbo ); 
	void						BindIndexStream( IndexBuffer *ibo );  // Be able to bind index buffers;   

	// Uniform/Constant Data
	void						BindModelMatrix( Matrix44 const &model );
	void						UpdateFrameBuffer();
	void						UpdateFxBuffer(float effectStrenght);
	void						BindUniformBuffer(uint slot, UniformBuffer *ubo);

	//Utility for copying between Texture2Ds (For full screen FX)
	void						CopyTexture(Texture2D *dst, Texture2D *src);   // A10

	//Draw Calls	
	void						Draw(uint vertexCount, uint byteOffset = 0U);
	void						DrawIndexed( uint indexCount);                                 
	void						DrawVertexArray( Vertex_PCU const *vertices, uint count ); 
	void						DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void						DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);
	void						DrawMesh( GPUMesh *mesh );                                         
	
	//Full screen effects helpers
	void						ApplyEffect(Material *mat);
	void						ApplyEffect(Texture2D *dst, Texture2D *src, Material *mat); // A10 

	IntVec2						GetCurrentScreenDimensions();

	void						RequestScreenshot();
	void						CreateStagingTexture(Texture2D* stagingTexture, Texture2D* backBuffer);
public:
	//White texture
	unsigned char				plainWhiteImageData[4] = {255, 255, 255, 255};

private:
	//D3D11 Functions
	bool						D3D11Setup(void* hwnd);   // Creates required D3D Objects
	void						D3D11Cleanup();          // Cleans up D3D11 Objects

	void						Startup();
	void						PremakeDefaults();

	void						DemoRender();            // Does rendering for this demo

	bool						PreDraw(GPUMesh *mesh);

	// Private (internal) member functions will go here
	BitmapFont*					CreateBitmapFontFromFile(const std::string& bitmapName);
	Shader*						CreateShaderFromFile(const std::string& fileName);

private:
	// Private (internal) data members will go here
	std::map< std::string, BitmapFont* >				m_loadedFonts;
	std::map< std::string, Shader*>						m_loadedShaders;
	std::map<std::string, TextureView*>					m_cachedTextureViews;
	std::map<std::string, Material*>					m_materialDatabase;
	std::map<std::string, GPUMesh*>						m_modelDatabase;

	ID3D11Device										*m_D3DDevice = nullptr;
	ID3D11DeviceContext									*m_D3DContext = nullptr;
	IDXGISwapChain										*m_D3DSwapChain = nullptr;
	ID3D11RasterizerState								*m_defaultRasterState = nullptr; 

	//CTV holding reference to the back_buffer
	ColorTargetView*									m_FrameBuffer_ColorTargetView = nullptr;

	Camera*												m_currentCamera = nullptr;
	Shader*												m_currentShader = nullptr;
	
	VertexBuffer*										m_immediateVBO = nullptr;
	UniformBuffer*										m_immediateUBO = nullptr;
	
	Texture2D*											m_defaultDepthTexture = nullptr;
	DepthStencilTargetView*								m_defaultDepthStencilView = nullptr;
	Texture2D*											m_defaultColorTexture = nullptr;
	ColorTargetView*									m_defaultColorTargetView = nullptr;

	//For FX
	Texture2D*											m_FXTexture = nullptr;
	ColorTargetView*									m_FXColorTargetView = nullptr;

	UniformBuffer*										m_modelBuffer = nullptr;

	// constant buffer storing lights; 
	UniformBuffer*										m_gpuLightBuffer = nullptr;
	

	Sampler*											m_cachedSamplers[SAMPLE_MODE_COUNT];

	GPUMesh*											m_immediateMesh = nullptr;
	ModelBufferT										m_cpuModelBuffer;

	//Full screen effects
	Camera*												m_FXCam = nullptr;

	void*												m_hwnd = nullptr;

	bool												m_screenShotRequested = false;

	Texture2D*											m_stagingTexture = nullptr;
public:

	// CPU copy of light data
	LightBufferT										m_cpuLightBuffer;
	// need to update the light buffer before a draw
	bool												m_lightBufferDirty = true;
	uint												m_lightSlot = 0U;

	unsigned int										m_frameCount = 0;

	Image*												m_whiteImage = nullptr;
};


