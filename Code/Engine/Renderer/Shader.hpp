#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include <string>

struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;
struct ID3D11InputLayout;
struct ID3D11BlendState;

class RenderContext;

// A programmable shader stage, either vertex or pixel fornow; 
class ShaderStage
{
public:
	ShaderStage();
	~ShaderStage();

public:
	bool LoadShaderFromSource( RenderContext *renderContext, const std::string &fileName, void const *source, unsigned long sourceSize, eShaderStage stage );

	eShaderStage m_stage; 
	union {
		ID3D11Resource *m_handle; 
		ID3D11VertexShader *m_vs; 
		ID3D11PixelShader *m_ps; 
	};
	ID3D10Blob *m_byteCode = nullptr;
	RenderContext *m_owningRenderContext = nullptr;

	inline bool IsValid() const { return m_handle != nullptr; }
}; 

//------------------------------------------------------------------------
class Shader 
{
public:
	Shader();
	~Shader();
	
	static char const*		GetEntryForStage(eShaderStage stage);
	static char const*		GetShaderModelForStage(eShaderStage stage );

	bool					CreateInputLayoutForVertexPCU(); 
	bool					UpdateBlendStateIfDirty(RenderContext *renderContext); 

	void					SetBlendMode( eBlendMode mode ); 

public:
	
	ShaderStage				m_vertexStage; 
	ShaderStage				m_pixelStage; 

	// blend state - not sure how your SD1 exposes this.  Some Engines expose the blend factors
	// and some engines will expose an enum for common setups (like alpha, additive, etc...)
	eBlendMode				m_blendMode = BLEND_MODE_ALPHA; 

	// eBlendFactor m_srcFactor;  
	// eBlendFactor m_dstFactor; 
	bool					m_blendStateDirty = true; 


	ID3D11InputLayout*		m_inputLayout = nullptr; 
	ID3D11BlendState*		m_blendState = nullptr; 
}; 

ID3D10Blob* CompileHLSLToShaderBlob( char const *opt_filename,		// optional: used for error messages
	void const* source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const* entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target  );                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)