//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
#define MAX_LIGHTS      (8)

// As the engine designer, we're going to make the decision to 
// just reserve certain slot indices for specific constant buffers
// We will group uniform buffers by how often they change, and for now, we
// really only use the CAMERA
enum eCoreUniformSlot 
{
	UNIFORM_SLOT_FRAME			= 1,
	UNIFORM_SLOT_CAMERA			= 2, 
	UNIFORM_SLOT_MODEL_MATRIX	= 3,
	UNIFORM_SLOT_LIGHT			= 4,
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
	Matrix44 ViewMatrix; 
	Matrix44 ProjectionMatrix;
	Vec3 CameraPosition;
	float pad;
};

//------------------------------------------------------------------------------------------------------------------------------
// slot 3
// This is going to be the model matrix. This is gonna be updated on each draw call for each of the objects 
// in the scene that we wish to render.
struct ModelBufferT
{
	Matrix44 ModelMatrix;
	Rgba TintColor = Rgba::WHITE;
};

// I start at slot 1 out of habit.  I reserve slot 0 for what I call the "SYTEM" buffer, which
// is usually just debug constants and configurations as I'm working. 
// The uniform buffer system is very free-form, you can bind any data to any slot
// but I like to usually have my engine renderer reserve a few slots
// grouped by update frequency.

// A enum to pick a sampler state to use
enum eSampleMode
{
	SAMPLE_MODE_POINT       = 0, 
	SAMPLE_MODE_LINEAR, 
	// SAMPLE_MODE_BILINEAR

	SAMPLE_MODE_COUNT,
	SAMPLE_MODE_DEFAULT     = SAMPLE_MODE_LINEAR // different games may want to change this.  My projects will use Linear, SD4 will want point; 
}; 

// We have to specify what can a buffer be used 
// for.  It is possible for a buffer to serve multiple purposes
// though for our case, we'll be special casing them for clarity; 
enum eRenderBufferUsageBit : uint
{
	RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT   = BIT_FLAG(0),   // Can be bound to an vertex input stream slot
	RENDER_BUFFER_USAGE_INDEX_STREAM_BIT    = BIT_FLAG(1),   // Can be bound as an index input stream.  
	RENDER_BUFFER_USAGE_UNIFORM_BIT         = BIT_FLAG(2),   // Can be bound to a constant buffer slot; 
};
typedef uint eRenderBufferUsageBits; 

// In D3D11 and GL - buffers also are supplied hints on 
// desired usage patterns to help the GPU optimize for it
enum eGPUMemoryUsage 
{
	GPU_MEMORY_USAGE_GPU,     // Can be written/read from GPU only (Color Targets are a good example)
	GPU_MEMORY_USAGE_STATIC,  // Created, and are read only after that (ex: textures from images, sprite atlas)
	GPU_MEMORY_USAGE_DYNAMIC, // Update often from CPU, used by the GPU (CPU -> GPU updates, used by shaders.  ex: Uniform Buffers)
	GPU_MEMORY_USAGE_STAGING, // For getting memory from GPU to CPU (can be copied into, but not directly bound as an output.  ex: Screen shots system)
}; 

// Eventually, when we add new Texture types, these common 
// enums & classes should be moved to Texture.hpp
enum eTextureUsageBit : uint
{
	TEXTURE_USAGE_TEXTURE_BIT              = BIT_FLAG(0),    // Can be used to create a TextureView
	TEXTURE_USAGE_COLOR_TARGET_BIT         = BIT_FLAG(1),    // Can be used to create a ColorTargetView
	TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT = BIT_FLAG(2),    // Can be used to create a DepthStencilTargetView
}; 
typedef uint eTextureUsageBits; 

enum eShaderStage
{
	SHADER_STAGE_VERTEX, 
	SHADER_STAGE_FRAGMENT, 
};

// two options for this - either you can name common blend mode configurations
// using an enum, or you can fully expose it using a color and alpha blend operation; 
// Believe the enum is what Squirrel is intended for your engines; 
enum eBlendMode
{
	// op,  src_factor,  dst_factor
	BLEND_MODE_OPAQUE,   // ADD, ONE,         ZERO              (or blend disabled)
	BLEND_MODE_ALPHA,    // ADD, SRC_ALPHA,   INV_SRC_ALPHA,
	BLEND_MODE_ADDTIVE,  // ADD, ONE,         ONE
	BLEND_MODE_CUSTOM,	 // Setup based on XML Values read
}; 

// other options - fully expose the options; 
// enum eBlendOp { ... }; // maps to https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ne-d3d11-d3d11_blend_op
enum eBlendOperation
{
	BLEND_OP_ADD,
	BLEND_OP_MAX,
	BLEND_OP_LEQUAL,
	BLEND_OP_GREATER,

	BLEND_OP_DEFAULT = BLEND_OP_ADD
};

// enum eBlendFactor { ... }; // maps to https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ne-d3d11-d3d11_blend
enum eBlendFactor
{
	FACTOR_SOURCE_ALPHA,
	FACTOR_INV_SOURCE_ALPHA,
	FACTOR_INV_DEST_ALPHA,
	FACTOR_ONE,
	FACTOR_ZERO,
};

// struct blend_info_t { eBlendOp op; eBlendFactor srcFactor;  eBlendFactor dstFactor; }; 

enum eCompareOp            // A04
{
	COMPARE_NEVER,       // false,      "never"
	COMPARE_ALWAYS,      // true,       "always"
	COMPARE_EQUAL,       // ==,         "equal"
	COMPARE_NOTEQUAL,    // !=,         "not"
	COMPARE_LESS,        // <           "less"
	COMPARE_LEQUAL,      // <=          "lequal"
	COMPARE_GREATER,     // >           "greater"
	COMPARE_GEQUAL,      // >=          "gequal"
}; 

//------------------------------------------------------------------------------------------------------------------------------
enum eDataFormat
{
	DF_NULL, 

	DF_FLOAT, 
	DF_VEC2, 
	DF_VEC3, 
	DF_RGBA32, 
};

//------------------------------------------------------------------------------------------------------------------------------
// used by light buffer (A06)
struct LightT 
{
	Rgba color					= Rgba::BLACK; // a is intensity; 

	Vec3 position				= Vec3::ZERO;                   // light position
	float pad00					= 0.f;                     // keep a 16-byte alignment for D3D11

	Vec3 direction				= Vec3( 0.f, 0.f, 1.f);                  // direction of light
	float isDirection			= 0.0f; // 0 means use as a point light 

	Vec3 diffuseAttenuation		= Vec3( 1.0f, 0.0f, 0.0f );  // no attenuation
	float pad10					= 0.f;

	Vec3 specularAttenuation	= Vec3( 1.0f, 0.0f, 0.0f ); // no attenuation
	float pad20					= 0.f; 
}; 

//------------------------------------------------------------------------------------------------------------------------------
// slot 4, lights (A06)
struct LightBufferT
{
	Rgba ambient; // a is intensity
	float specFactor = 1.f; 
	float specPower = 64.f; 
	Vec2 pad; 

	LightT lights[MAX_LIGHTS]; 
}; 