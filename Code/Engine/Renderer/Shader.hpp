#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"
#include <d3d11.h>
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
	std::string		m_stageEntry = "";				

	inline bool IsValid() const { return m_handle != nullptr; }
}; 

//------------------------------------------------------------------------
class Shader 
{
public:
	STATIC D3D11_BLEND_OP			DXUsageFromBlendOp( eBlendOperation const usage );
	STATIC D3D11_BLEND				DXUsageFromBlendFactor( eBlendFactor const usage );
	STATIC D3D11_COMPARISON_FUNC	DXGetCompareFunction( eCompareOp const usage );
	Shader();
	~Shader();
	
	static char const*		GetEntryForStage(eShaderStage stage);
	static char const*		GetShaderModelForStage(eShaderStage stage );

	static eBlendOperation	GetBlendOpForBlendMode(eBlendMode blendMode);
	static eBlendFactor		GetSourceFactorForBlendMode(eBlendMode blendMode);
	static eBlendFactor		GetDestFactorForBlendMode(eBlendMode blendMode);

	// XML Utilities for Shader
	void					LoadShaderFromXMLSource( const std::string &fileName);
	void					SetBlendDataFromString();
	void					SetDepthOpFromString();
	eBlendOperation			SetOpFromString( const std::string& blendOp );
	eBlendFactor			SetFactorFromString( const std::string& blendFactor );
	void					ReadFromPass( XMLElement& passEntry );
	
	// Create Input layouts based on what type of buffer we are passing
	bool					CreateInputLayoutForVertexPCU(); 
	bool					CreateInputLayoutForBufferLayout();


	// Depth stencil state now also needs to be generated; 
	bool					UpdateBlendStateIfDirty(RenderContext *renderContext); 
	bool					UpdateDepthStateIfDirty(RenderContext *renderContext);

	// Can now set our depth compare mode; 
	void					SetDepth( eCompareOp op, bool write );                 // A04
	void					SetBlendMode( eBlendMode mode ); 

public:
	
	ShaderStage				m_vertexStage; 
	ShaderStage				m_pixelStage; 

	// blend state - Some Engines expose the blend factors
	// and some engines will expose an enum for common setups (like alpha, additive, etc...)
	eBlendMode				m_blendMode = BLEND_MODE_ALPHA; 
	eBlendOperation			m_blendOp = BLEND_OP_ADD;

	//For color
	eBlendFactor			m_srcFactor = FACTOR_SOURCE_ALPHA;  
	eBlendFactor			m_dstFactor = FACTOR_INV_SOURCE_ALPHA; 

	//For alpha
	eBlendFactor			m_srcAlphaFactor = FACTOR_ONE;
	eBlendFactor			m_dstAlphaFactor = FACTOR_ONE;
	eBlendOperation			m_alphaBlendOp = BLEND_OP_MAX;

	bool					m_blendStateDirty = true; 
	bool					m_depthStateDirty = true;              // A04

	// Depth/Stencil Mode
	eCompareOp				m_depthCompareOp = COMPARE_LEQUAL;    // A04
	bool					m_writeDepth = false;             // A04

	//XML Parsed information
	bool			m_customMode = false;
	std::string		m_shaderSourcePath = "";
	std::string		m_depthCompareOpString = "";
	std::string		m_blendOpString = "";
	std::string		m_blendSrcString = "";
	std::string		m_blendDstString = "";
	std::string		m_blendOpAlphaString = "";
	std::string		m_blendSrcAlphaString = "";
	std::string		m_blendDstAlphaString = "";

	ID3D11InputLayout*			m_inputLayout = nullptr; 
	ID3D11BlendState*			m_blendState = nullptr; 
	ID3D11DepthStencilState*	m_depthStencilState = nullptr;     // A04
}; 

ID3D10Blob* CompileHLSLToShaderBlob( char const *opt_filename,		// optional: used for error messages
	void const* source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const* entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target  );                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)