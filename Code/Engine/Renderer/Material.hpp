#pragma once
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
enum eTextureSlots
{
	DIFFUSE_SLOT = 0U,
	NORMAL_SLOT = 1U,
	EMISSIVE_SLOT = 2U,
	SPECULAR_SLOT = 3U
};

//------------------------------------------------------------------------------------------------------------------------------
class Material
{
public:
	explicit Material( RenderContext *renderContext ); 
	explicit Material( RenderContext *renderContext, const std::string& fileName ); 
	~Material();

	void				LoadMaterialFromXML( const std::string& fileName );

	void				SetShader( Shader *shader ); 
	void				SetShader( char const *shader_name ); 

	void				SetTextureView( uint slot, TextureView *view ); 
	void				SetTextureView( uint slot, char const *name );

	void				SetSampler( uint slot, Sampler *sampler ); 

	TextureView*		GetTextureView( uint slot ) const; 
	Sampler*			GetSampler( uint slot ) const; 

	// Convenience Helpers - meaning it can pick a sane default when set to nullptr;
	void				SetDiffuseMap( TextureView *view );  // default to "white" on nullptr;
	void				SetNormalMap( TextureView *view );   // default to "flat" on nullptr;
	void				SetSpecularMap( TextureView *view ); // default to "white" on nullptr;
	void				SetEmissiveMap( TextureView *view ); // default to "black" on nullptr; 

	void				SetUniforms( void const *data, size_t const size );
	void				SetUniforms( UniformBuffer *ubo ); 
	UniformBuffer*		GetUniformBuffer() const;

	int					GetNumTextures();
	int					GetNumSamplers();
	// templated SetUniforms could be useful as well
	// ...

public:
	Shader*						m_shader = nullptr; 
	std::vector<TextureView*>	m_textures; 
	std::vector<Sampler*>		m_samplers; 

	UniformBuffer*				m_materialBuffer = nullptr; 

private:
	//XML string names
	std::string					m_materialName = "";
	std::string					m_shaderName = "";
	std::string					m_diffuseName = "";
	std::string					m_normalName = "";
	std::string					m_specName = "";
	std::string					m_samplerName = "";
	std::string					m_emissiveName = "";

	int							m_samplerIndex = 0;
	std::string					m_samplerType = "";

	RenderContext*				m_renderContext = nullptr;
};