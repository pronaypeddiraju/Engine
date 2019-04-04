#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"

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
#define DEBUG_SHADERS
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

STATIC D3D11_BLEND_OP Shader::DXUsageFromBlendOp( eBlendOperation const usage )
{
	switch (usage) 
	{
	case BLEND_OP_ADD:      return D3D11_BLEND_OP_ADD;
	case BLEND_OP_MAX:		return D3D11_BLEND_OP_MAX;
	default:
	{
		GUARANTEE_RECOVERABLE(false, "Setting D3D11 Blend op to ADD"); 
		return D3D11_BLEND_OP_ADD;
	}
	}
}

STATIC D3D11_BLEND Shader::DXUsageFromBlendFactor( eBlendFactor const usage)
{
	switch (usage) 
	{
	case FACTOR_ONE:				return D3D11_BLEND_ONE;
	case FACTOR_SOURCE_ALPHA:		return D3D11_BLEND_SRC_ALPHA;
	case FACTOR_INV_SOURCE_ALPHA:	return D3D11_BLEND_INV_SRC_ALPHA;
	case FACTOR_ZERO:				return D3D11_BLEND_ZERO;
	case FACTOR_INV_DEST_ALPHA:		return D3D11_BLEND_INV_DEST_ALPHA;
	default:
	{
		GUARANTEE_RECOVERABLE(false, "Setting D3D11 Blend factor to ONE"); 
		return D3D11_BLEND_ONE;
	}
	}
}

STATIC D3D11_COMPARISON_FUNC Shader::DXGetCompareFunction( eCompareOp const usage)
{
	switch (usage) 
	{
	case COMPARE_NEVER:				return D3D11_COMPARISON_NEVER;
	case COMPARE_ALWAYS:			return D3D11_COMPARISON_ALWAYS;
	case COMPARE_EQUAL:				return D3D11_COMPARISON_EQUAL;
	case COMPARE_NOTEQUAL:		    return D3D11_COMPARISON_NOT_EQUAL;
	case COMPARE_LESS:				return D3D11_COMPARISON_LESS;
	case COMPARE_LEQUAL:			return D3D11_COMPARISON_LESS_EQUAL;
	case COMPARE_GREATER:			return D3D11_COMPARISON_GREATER;
	case COMPARE_GEQUAL:			return D3D11_COMPARISON_GREATER_EQUAL;
	default:
	{
		GUARANTEE_RECOVERABLE(false, "Setting D3D11 Depth op to Always"); 
		return D3D11_COMPARISON_LESS_EQUAL;
	}
	}
}

STATIC DXGI_FORMAT Shader::GetDXDataFormat( eDataFormat const format )
{
	switch( format )
	{
	case DF_RGBA32:					return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case DF_VEC2:					return DXGI_FORMAT_R32G32_FLOAT;
	case DF_VEC3:					return DXGI_FORMAT_R32G32B32_FLOAT;
	case DF_NULL:					ERROR_AND_DIE("The format recieved was null");
	default:
	{
		ERROR_AND_DIE("DXDataFormat undefined!"); 
	}
	break;
	}
}

Shader::Shader()
{

}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
	DX_SAFE_RELEASE(m_blendState);
	DX_SAFE_RELEASE(m_depthStencilState);
}

bool Shader::UpdateBlendStateIfDirty( RenderContext *renderContext )
{
	// if we already have a good blend state - keep it; 
	if ((m_blendState != nullptr) && (false == m_blendStateDirty)) {
		return true; 
	}

	// Free old state
	DX_SAFE_RELEASE(m_blendState); 

	// otherwise, let's make one; 
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc)); 

	desc.AlphaToCoverageEnable = false;  // used in MSAA to treat alpha as coverage (usual example is foliage rendering, we will not be using this)
	desc.IndependentBlendEnable = false;   // if you have multiple outputs bound, you can set this to true to have different blend state per output

	if(m_blendMode != BLEND_MODE_CUSTOM)
	{
		m_srcFactor = GetSourceFactorForBlendMode(m_blendMode);
		m_dstFactor = GetDestFactorForBlendMode(m_blendMode);
		m_blendOp = GetBlendOpForBlendMode(m_blendMode);
	}

	//TODO: Add logic to setup blend alpha for various operations. Ask Forseth about this

	// Blending is setting put the equation...
	// FinalColor = BlendOp( SrcFactor * outputColor, DestFactor * destColor )
	// where outputColor is what the pixel shader outputs
	// and destColor is the color already in the pixel shader

	// the below describes the equation...
	// FinalColor = outputColor.a * outputColor + (1.0f - outputColor.a) * destColor;  

	// since we disabled independent blend, we only have to setup the first blend state
	// and I'm setting it up for "alpha blending"
	desc.RenderTarget[0].BlendEnable = TRUE;  // we want to blend
	desc.RenderTarget[0].SrcBlend    = DXUsageFromBlendFactor(m_srcFactor);      // output color is multiplied by the output colors alpha and added to...
	desc.RenderTarget[0].DestBlend   = DXUsageFromBlendFactor(m_dstFactor);  // the current destination multiplied by (1 - output.a); 
	desc.RenderTarget[0].BlendOp     = DXUsageFromBlendOp(m_blendOp);        // we add the two results together

	// you can compute alpha seperately, in this case, we'll just set it to be the max alpha between the src & destination
	desc.RenderTarget[0].SrcBlendAlpha  = DXUsageFromBlendFactor(m_srcAlphaFactor);
	desc.RenderTarget[0].DestBlendAlpha = DXUsageFromBlendFactor(m_dstAlphaFactor); 
	desc.RenderTarget[0].BlendOpAlpha   = DXUsageFromBlendOp(m_alphaBlendOp);

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // can mask off outputs;  we won't be doing that; 
	
	// Finally, create the blend state
	renderContext->m_D3DDevice->CreateBlendState( &desc, &m_blendState);

	m_blendStateDirty = false; 
	return (m_blendState != nullptr); 
}

bool Shader::UpdateDepthStateIfDirty( RenderContext *renderContext )
{
	if (m_depthStateDirty || (m_depthStencilState == nullptr)) 
	{
		D3D11_DEPTH_STENCIL_DESC ds_desc = {};

		ds_desc.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and compare is always)
		ds_desc.DepthWriteMask = m_writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO; 
		ds_desc.DepthFunc = DXGetCompareFunction( m_depthCompareOp ); //  

		// Stencil - just use defaults for now; 
		ds_desc.StencilEnable = false; 
		ds_desc.StencilReadMask = 0xFF; 
		ds_desc.StencilWriteMask = 0xFF; 

		D3D11_DEPTH_STENCILOP_DESC default_stencil_op = {}; 
		default_stencil_op.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
		default_stencil_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
		default_stencil_op.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
		default_stencil_op.StencilFunc = D3D11_COMPARISON_ALWAYS;      // function to test against

		// can have different rules setup for front and back face
		ds_desc.FrontFace = default_stencil_op; 
		ds_desc.BackFace = default_stencil_op; 

		DX_SAFE_RELEASE(m_depthStencilState); 
		renderContext->m_D3DDevice->CreateDepthStencilState( &ds_desc, &m_depthStencilState ); 
		m_depthStateDirty = false; 
		return true;
	}
	else
	{
		return false;
	}
}

void Shader::SetDepth( eCompareOp op, bool write )
{
	if(m_depthCompareOp != op || m_writeDepth != write)
	{
		m_depthCompareOp = op;
		m_writeDepth = write;

		m_depthStateDirty = true;
	}
}

void Shader::SetBlendMode( eBlendMode mode )
{
	if(m_blendMode != mode)
	{
		m_blendMode = mode;
		m_blendStateDirty = true;
	}
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

bool Shader::CreateInputLayout(const BufferLayout* layout)
{
	if(layout == m_bufferLayout)
	{
		return true;
	}

	//Free old inputLayout
	DX_SAFE_RELEASE( m_inputLayout ); 

	D3D11_INPUT_ELEMENT_DESC *inputDescription = new D3D11_INPUT_ELEMENT_DESC[layout->GetAttributeCount()];
	//D3D11_INPUT_ELEMENT_DESC inputDescription[3];

	memset( inputDescription, 0, sizeof(inputDescription) ); 

	int count = (int)layout->GetAttributeCount();
	for(int attrIdx = 0; attrIdx < count; attrIdx++)
	{
		// Map Position
		inputDescription[attrIdx].SemanticName = layout->m_attributes[attrIdx].m_name.c_str();             // __semantic__ name we gave this input -> float3 pos : POSITION; 
		inputDescription[attrIdx].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
		inputDescription[attrIdx].Format = Shader::GetDXDataFormat(layout->m_attributes[attrIdx].m_type);// DXGI_FORMAT_R32G32B32_FLOAT;  // Type this data is (float3/vec3 - so 3 floats)
		inputDescription[attrIdx].InputSlot = 0U;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
		inputDescription[attrIdx].AlignedByteOffset = (UINT) layout->m_attributes[attrIdx].m_memberOffset;   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
		inputDescription[attrIdx].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
		inputDescription[attrIdx].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

	}

	ID3D10Blob* vs_byteCode = m_vertexStage.m_byteCode; 

	ID3D11Device* device = m_vertexStage.m_owningRenderContext->m_D3DDevice;

	// Final create the layout
	HRESULT hr = device->CreateInputLayout( inputDescription, 
		(UINT) count,
		vs_byteCode->GetBufferPointer(), 
		vs_byteCode->GetBufferSize(), 
		&m_inputLayout );   
	
	delete[] inputDescription;

	m_bufferLayout = layout;

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


eBlendOperation Shader::GetBlendOpForBlendMode( eBlendMode blendMode )
{
	switch (blendMode)
	{
	case BLEND_MODE_ALPHA:				return BLEND_OP_ADD;
	case BLEND_MODE_ADDTIVE:			return BLEND_OP_ADD;
	case BLEND_MODE_OPAQUE:				return BLEND_OP_ADD;
	default: GUARANTEE_OR_DIE(false, "Unknown Blend Mode"); 
	}
}

eBlendFactor Shader::GetSourceFactorForBlendMode( eBlendMode blendMode )
{
	switch (blendMode)
	{
	case BLEND_MODE_ALPHA:				return FACTOR_SOURCE_ALPHA;
	case BLEND_MODE_ADDTIVE:			return FACTOR_ONE;
	case BLEND_MODE_OPAQUE:				return FACTOR_ONE;
	default: GUARANTEE_OR_DIE(false, "Unknown Blend Mode"); 
	}
}

eBlendFactor Shader::GetDestFactorForBlendMode( eBlendMode blendMode )
{
	switch (blendMode)
	{
	case BLEND_MODE_ALPHA:				return FACTOR_INV_SOURCE_ALPHA;
	case BLEND_MODE_ADDTIVE:			return FACTOR_ONE;
	case BLEND_MODE_OPAQUE:				return FACTOR_ZERO;
	default: GUARANTEE_OR_DIE(false, "Unknown Blend Mode"); 
	}
}

void Shader::LoadShaderFromXMLSource(const std::string &fileName )
{
	//Open the xml file and parse it
	tinyxml2::XMLDocument shaderDoc;
	shaderDoc.LoadFile(fileName.c_str());

	if(shaderDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		/*
		printf("\n >> Error loading XML file from %s ", fileName);
		printf("\n >> Error ID : %i ", shaderDoc.ErrorID());
		printf("\n >> Error line number is : %i", shaderDoc.ErrorLineNum());
		printf("\n >> Error name : %s", shaderDoc.ErrorName());
		*/
		ERROR_AND_DIE(">> Error loading Shader XML file ");
		return;
	}
	else
	{
		//We read everything fine. Now just shove all that data into the required place
		XMLElement* rootElement = shaderDoc.RootElement();
		
		//Read from Pass
		rootElement = rootElement->FirstChildElement();
		ReadFromPass(*rootElement);
		
		//Set vertex entry function
		rootElement = rootElement->FirstChildElement();
		m_vertexStage.m_stageEntry = ParseXmlAttribute(*rootElement, "entry", "");
		
		//Set fragement entry function
		rootElement = rootElement->NextSiblingElement();
		m_pixelStage.m_stageEntry = ParseXmlAttribute(*rootElement, "entry", "");

		//Set depth data
		rootElement = rootElement->NextSiblingElement();
		m_writeDepth = ParseXmlAttribute(*rootElement, "write", m_writeDepth);
		m_depthCompareOpString = ParseXmlAttribute(*rootElement, "test", m_depthCompareOpString);

		//Set Blend data
		rootElement = rootElement->NextSiblingElement();
		XMLElement* childElement = rootElement->FirstChildElement();
		//See if it's a custom blend mode.
		m_customMode = ParseXmlAttribute(*childElement, "customMode", m_customMode);
		if(m_customMode)
		{
			m_blendMode = BLEND_MODE_CUSTOM;
		}

		childElement = childElement->NextSiblingElement();
		m_blendOpString = ParseXmlAttribute(*childElement, "op", m_blendOpString);
		m_blendSrcString = ParseXmlAttribute(*childElement, "src", m_blendSrcString);
		m_blendDstString= ParseXmlAttribute(*childElement, "dst", m_blendDstString);

		childElement = childElement->NextSiblingElement();
		m_blendOpAlphaString = ParseXmlAttribute(*childElement, "op", m_blendOpAlphaString);
		m_blendSrcAlphaString = ParseXmlAttribute(*childElement, "src", m_blendSrcAlphaString);
		m_blendDstAlphaString = ParseXmlAttribute(*childElement, "dst", m_blendDstAlphaString);
	}

	SetDepthOpFromString();
	SetBlendDataFromString();
}

void Shader::SetBlendDataFromString()
{
	//Set Blend Op
	m_blendOp = SetOpFromString(m_blendOpString);

	//Set Blend Factors
	m_srcFactor = SetFactorFromString(m_blendSrcString);
	m_dstFactor = SetFactorFromString(m_blendDstString);

	//Set Blend Alpha Op
	m_alphaBlendOp = SetOpFromString(m_blendOpAlphaString);

	//Set Blend Factors
	m_srcAlphaFactor = SetFactorFromString(m_blendSrcAlphaString);
	m_dstAlphaFactor = SetFactorFromString(m_blendDstAlphaString);
}

eBlendOperation Shader::SetOpFromString(const std::string& blendOp)
{
	if(blendOp == "add")			{		return BLEND_OP_ADD;		}
	else if(blendOp == "max")		{		return BLEND_OP_MAX;		}
	else if(blendOp == "lequal")	{		return BLEND_OP_LEQUAL;		}
	else if(blendOp == "greater")	{		return BLEND_OP_GREATER;	}
	else
	{
		// Add an assert here; 
		ERROR_AND_DIE("Blend operation not yet handled");
	}
}

eBlendFactor Shader::SetFactorFromString(const std::string& blendFactor)
{
	if(blendFactor == "one")
	{
		return FACTOR_ONE;
	}
	else if(blendFactor == "zero")
	{
		 return FACTOR_ZERO;
	}
	else if(blendFactor == "inv_src_alpha")
	{
		return FACTOR_INV_SOURCE_ALPHA;
	}
	else if(blendFactor == "src_alpha")
	{
		return FACTOR_SOURCE_ALPHA;
	}
	else if(blendFactor == "inv_dst_alpha")
	{
		return FACTOR_INV_DEST_ALPHA;
	}
	else
	{
		// Add an assert here; 
		GUARANTEE_RECOVERABLE(false, "Blend Factor not yet handled");
		return FACTOR_ONE;
	}
}

void Shader::SetDepthOpFromString()
{
	if(m_depthCompareOpString == "lequal")
	{
		m_depthCompareOp = COMPARE_LEQUAL;
	}
	else if(m_depthCompareOpString == "equal")
	{
		m_depthCompareOp = COMPARE_EQUAL;
	} 
	else 
	{
		// Add an assert here; 
		GUARANTEE_RECOVERABLE(false, "Depth compare operation not yet handled");
	}

	//Add other depth operations when you start using them here
}

void Shader::ReadFromPass( XMLElement& passEntry )
{
	m_shaderSourcePath = ParseXmlAttribute(passEntry, "src", m_shaderSourcePath);
}

bool ShaderStage::LoadShaderFromSource( RenderContext *renderContext, const std::string &fileName, void const *source, unsigned long sourceSize, eShaderStage stage )
{
	m_stage = stage; 
	m_owningRenderContext = renderContext;
	ID3D11Device *device = renderContext->m_D3DDevice; 

	const char* stageEntry;
	if(m_stageEntry == "")
	{
		stageEntry = Shader::GetEntryForStage(stage);
	}
	else
	{
		stageEntry = m_stageEntry.c_str();
	}


	m_byteCode = CompileHLSLToShaderBlob( fileName.c_str(), source, sourceSize, stageEntry, Shader::GetShaderModelForStage(stage) ); 
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

