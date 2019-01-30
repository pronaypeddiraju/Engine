#include "Engine/Renderer/Shader.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"

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
}

ID3DBlob* CompileHLSLToShaderBlob( char const *opt_filename,  // optional: used for error messages
	void const *source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
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

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

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

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *error_string = (char*) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s", 
				opt_filename, 
				error_string );
			DX_SAFE_RELEASE(errors);
		} else {
			DebuggerPrintf( "Failed with HRESULT: %u", hr ); 
		}
	} 

	// will be nullptr if it failed to compile
	return code;
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


bool ShaderStage::LoadShaderFromSource( RenderContext *renderContext, std::string &fileName, void const *source, unsigned long sourceSize, eShaderStage stage )
{
	m_stage = stage; 
	ID3D11Device *device = renderContext->m_D3DDevice; 

	ID3DBlob *bytecode = CompileHLSLToShaderBlob( fileName.c_str(), source, sourceSize, Shader::GetEntryForStage(stage), Shader::GetShaderModelForStage(stage) ); 
	if (bytecode == nullptr) {
		return false; 
	}

	switch (stage) 
	{
	case SHADER_STAGE_VERTEX:    // Compile the byte code to the final shader (driver/hardware specific program)
	device->CreateVertexShader( bytecode->GetBufferPointer(), 
		bytecode->GetBufferSize(), 
		nullptr, 
		&m_vs );
	break;

	case SHADER_STAGE_FRAGMENT: 
	device->CreatePixelShader( bytecode->GetBufferPointer(), 
		bytecode->GetBufferSize(), 
		nullptr, 
		&m_ps );
	break; 
	}


	DX_SAFE_RELEASE(bytecode); 
	return IsValid();
}


