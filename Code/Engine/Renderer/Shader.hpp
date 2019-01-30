#pragma once
#include <string>

struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;

class RenderContext;

enum eShaderStage
{
	SHADER_STAGE_VERTEX, 
	SHADER_STAGE_FRAGMENT, 
};

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

	inline bool IsValid() const { return m_handle != nullptr; }
}; 

//------------------------------------------------------------------------
class Shader 
{

public:
	ShaderStage m_vertex_shader; 
	ShaderStage m_pixel_shader; 

	static char const* GetEntryForStage(eShaderStage stage);
	static char const* GetShaderModelForStage(eShaderStage stage );

}; 

ID3D10Blob* CompileHLSLToShaderBlob( char const *opt_filename,		// optional: used for error messages
	void const *source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target  );                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)