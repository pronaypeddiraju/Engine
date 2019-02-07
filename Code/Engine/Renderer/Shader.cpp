#include "Engine/Renderer/Shader.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Vertex_PCU.hpp"

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

// NEEDED FOR COMPILING
// Note:  This is not allowed for Windows Store Apps.
// Shaders must be compiled to byte-code offline. 
// but for development - great if the program knows how to compile it.
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

ShaderStage::ShaderStage()
	: m_handle(nullptr)
{

}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_handle); 
	DX_SAFE_RELEASE(m_byteCode);
	m_owningRenderContext = nullptr;
}

STATIC ID3D10Blob* CompileHLSLToShaderBlob( char const *opt_filename,  // optional: used for error messages
	void const* source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const* entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target  )                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0U;
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3D10Blob *code = nullptr;
	ID3D10Blob *errors = nullptr;

	HRESULT hr = ::D3DCompile( source_code, 
		source_code_size,                   // plain text source code
		opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
		compile_flags,                      // Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors );                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr))
	{
		if (errors != nullptr) 
		{
			char *error_string = (char*) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s", 
				opt_filename, 
				error_string );
			DX_SAFE_RELEASE(errors);
		} 
		else 
		{
			DebuggerPrintf( "Failed with HRESULT: %u", hr ); 
		}
	} 

	// will be nullptr if it failed to compile
	return code;
}


Shader::Shader()
{

}

Shader::~Shader()
{
	GUARANTEE_RECOVERABLE(true, "Dont forget to free inputLayout variable");
}

bool Shader::CreateInputLayoutForVertexPCU()
{
	// Early out - we've already created it; 
	// TODO: If vertex type changes, we need to rebind; 
	if (m_inputLayout != nullptr) 
	{
		return true; 
	}

	// This describes the input data to the shader
	// The INPUT_ELEMENT_DESC describes each element of the structure; 
	// Since we have POSITION, COLOR, UV, we need three descriptions; 
	D3D11_INPUT_ELEMENT_DESC inputDescription[3]; 

	// initialize memory to 0 (usually a sane default)
	memset( inputDescription, 0, sizeof(inputDescription) ); 

	// Map Position
	inputDescription[0].SemanticName = "POSITION";             // __semantic__ name we gave this input -> float3 pos : POSITION; 
	inputDescription[0].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
	inputDescription[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;  // Type this data is (float3/vec3 - so 3 floats)
	inputDescription[0].InputSlot = 0U;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
	inputDescription[0].AlignedByteOffset = offsetof( Vertex_PCU, m_position );   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
	inputDescription[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
	inputDescription[0].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)
	
	// YOU DO -> Map UV  
	inputDescription[1].SemanticName = "TEXCOORD";             // __semantic__ name we gave this input -> float3 pos : POSITION; 
	inputDescription[1].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
	inputDescription[1].Format = DXGI_FORMAT_R32G32_FLOAT;     // Type this data is (float3/vec3 - so 3 floats)
	inputDescription[1].InputSlot = 0U;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
	inputDescription[1].AlignedByteOffset = offsetof( Vertex_PCU, m_uvTexCoords);   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
	inputDescription[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
	inputDescription[1].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

	// YOU DO -> Map Color
	inputDescription[2].SemanticName = "COLOR";             // __semantic__ name we gave this input -> float3 pos : POSITION; 
	inputDescription[2].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
	inputDescription[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;     // Type this data is (float3/vec3 - so 3 floats)
	inputDescription[2].InputSlot = 0U;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
	inputDescription[2].AlignedByteOffset = offsetof( Vertex_PCU, m_color);   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
	inputDescription[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
	inputDescription[2].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)


	ID3D10Blob* vs_byteCode = m_vertexStage.m_byteCode; 

	ID3D11Device* device = m_vertexStage.m_owningRenderContext->m_D3DDevice;

	// Final create the layout
	HRESULT hr = device->CreateInputLayout( inputDescription, 
		ARRAYSIZE(inputDescription),
		vs_byteCode->GetBufferPointer(), 
		vs_byteCode->GetBufferSize(), 
		&m_inputLayout );   

	return SUCCEEDED(hr); 
}

STATIC char const* Shader::GetEntryForStage( eShaderStage stage ) 
{
	switch (stage) {
	case SHADER_STAGE_VERTEX: return "VertexFunction"; 
	case SHADER_STAGE_FRAGMENT: return "FragmentFunction";
	default: GUARANTEE_OR_DIE(false, "Unknown shader stage"); 
	}
}


STATIC char const* Shader::GetShaderModelForStage( eShaderStage stage ) 
{
	switch (stage) {
	case SHADER_STAGE_VERTEX: return "vs_5_0"; 
	case SHADER_STAGE_FRAGMENT: return "ps_5_0";
	default: GUARANTEE_OR_DIE(false, "Unknown shader stage"); 
	}
}


bool ShaderStage::LoadShaderFromSource( RenderContext *renderContext, const std::string &fileName, void const *source, unsigned long sourceSize, eShaderStage stage )
{
	m_stage = stage; 
	m_owningRenderContext = renderContext;
	ID3D11Device *device = renderContext->m_D3DDevice; 

	m_byteCode = CompileHLSLToShaderBlob( fileName.c_str(), source, sourceSize, Shader::GetEntryForStage(stage), Shader::GetShaderModelForStage(stage) ); 
	if (m_byteCode == nullptr) {
		return false; 
	}

	switch (stage) 
	{
	case SHADER_STAGE_VERTEX:    // Compile the byte code to the final shader (driver/hardware specific program)
	device->CreateVertexShader( m_byteCode->GetBufferPointer(), 
		m_byteCode->GetBufferSize(), 
		nullptr, 
		&m_vs );
	break;

	case SHADER_STAGE_FRAGMENT: 
	device->CreatePixelShader( m_byteCode->GetBufferPointer(), 
		m_byteCode->GetBufferSize(), 
		nullptr, 
		&m_ps );
	break; 
	}

	//We need to keep byte code for vertex shader to use in InputLayout to feed details
	//about the type of input being sent to the shader pipeline
	if(stage != SHADER_STAGE_VERTEX)
	{
		DX_SAFE_RELEASE(m_byteCode);
	}
	return IsValid();
}


