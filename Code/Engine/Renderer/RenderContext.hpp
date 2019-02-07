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
class RenderBuffer;
class UniformBuffer;
class VertexBuffer;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;

enum BlendMode
{
	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE
};

// As the engine designer, we're going to make the decision to 
// just reserve certain slot indices for specific constant buffers
// We will group uniform buffers by how often they change, and for now, we
// really only use the CAMERA
enum eCoreUniformSlot 
{
	UNIFORM_SLOT_FRAME         = 1,
	UNIFORM_SLOT_CAMERA        = 2, 
}; 

// A constant buffer can address up 64KB,
// with 15 available slots (docs usually say 14, as one gets used
// up by the $GLOBAL constant buffer; 

//------------------------------------------------------------------------
// FRAME data is information that is constant for an entire
// frame - you can update this UBO during RenderContext::BeginFrame, 
// as well as bind it (as long as you don't accidentally bind over it
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// I usually use this to store time, as it is a common
// input for shader effects
// A UBO should usually be a multiple of 16-bytes large, so we'll just add some 
// variations of time for our shaders to use; 
// slot: 1
struct FrameBufferT
{
	float frameCount; 
	float currentTime; 
	float cosine; 
	float sine; 
}; 

//------------------------------------------------------------------------
// slot 2; 
// What data does the camera provide the shader (slot:2)
// This must follow a 16-byte element alignment (for now, this only contains
// ortho information; 
// camera slot I usually bind all constants that change per view; 
struct CameraBufferT 
{
	Vec2 orthoMin; 
	Vec2 orthoMax; 
};

// I start at slot 1 out of habit.  I reserve slot 0 for what I call the "SYTEM" buffer, which
// is usually just debug constants and configurations as I'm working. 
// The uniform buffer system is very free-form, you can bind any data to any slot
// but I like to usually have my engine renderer reserve a few slots
// grouped by update frequency.

class RenderContext
{
	friend class ShaderStage;
	friend class Shader;
	friend class RenderBuffer;
	friend class UniformBuffer;

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
	Texture*			CreateOrGetTextureFromFile(const char* imageFilePath);
	BitmapFont*			CreateOrGetBitmapFontFromFile (const std::string& bitmapName);
	Shader*				CreateOrGetShaderFromFile(const std::string& fileName);

	void				BindShader(Shader* shader);
	void				SetBlendMode(BlendMode blendMode);
	void				BindTexture(Texture* texture);

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
	void				DemoRun();               // Demo setup and update loop

	// Private (internal) member functions will go here
	Texture*			CreateTextureFromFile( const char* imageFilePath);
	BitmapFont*			CreateBitmapFontFromFile(const std::string& bitmapName);
	Shader*				CreateShaderFromFile(const std::string& fileName);
	void				CreateAndSetDefaultRasterState();

private:
	// Private (internal) data members will go here
	std::map< std::string, Texture* >					m_loadedTextures;
	std::map< std::string, BitmapFont* >				m_loadedFonts;
	std::map< std::string, Shader*>						m_loadedShaders;

	ID3D11Device										*m_D3DDevice = nullptr;
	ID3D11DeviceContext									*m_D3DContext = nullptr;
	IDXGISwapChain										*m_D3DSwapChain = nullptr;
	ID3D11RasterizerState								*m_defaultRasterState = nullptr; 

	//For now make a global rtv
	//Wait no! we shouldnt be fucking with this
	//ID3D11RenderTargetView							*m_renderTargetView = nullptr;

	ColorTargetView*									m_FrameBuffer_ColorTargetView = nullptr;
	Camera*												m_currentCamera = nullptr;
	Shader*												m_currentShader = nullptr;

public:

	VertexBuffer*										m_immediateVBO = nullptr; 
	UniformBuffer*										m_immediateUBO = nullptr;

	unsigned int										m_frameCount = 0;


	//Sampler*											m_defaultPoint = nullptr; 
	//Sampler*											m_defaultBilinear = nullptr; 
};


