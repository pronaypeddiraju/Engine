#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Material::Material( RenderContext *renderContext )
{
	m_renderContext = renderContext;
}

Material::~Material()
{
	delete m_materialBuffer;
	m_materialBuffer = nullptr;
}

void Material::SetShader( Shader *shader )
{
	m_shader = shader;
}

void Material::SetShader( char const *shader_name )
{
	m_renderContext->CreateOrGetShaderFromFile(shader_name);
}

void Material::SetTextureView( uint slot, TextureView *view )
{
	uint numElements = static_cast<uint>(m_textures.size());

	if(numElements <= slot)
	{
		m_textures.resize(slot);
	}

	//Set the texture view at the received slot in vector
	m_textures[slot] = view;
}

void Material::SetTextureView( uint slot, char const *name )
{
	TextureView* texture = m_renderContext->GetOrCreateTextureViewFromFile(name);
	SetTextureView(slot, texture);
}

void Material::SetSampler( uint slot, Sampler *sampler )
{
	uint numElements = static_cast<uint>(m_samplers.size());

	if(numElements <= slot)
	{
		m_samplers.resize(slot);
	}

	//Set the sampler at the recieved slot in the vector
	m_samplers[slot] = sampler;
}

TextureView* Material::GetTextureView( uint slot ) const
{
	return m_textures[slot];
}

Sampler* Material::GetSampler( uint slot ) const
{
	return m_samplers[slot];
}

void Material::SetDiffuseMap( TextureView *view )
{
	SetTextureView(DIFFUSE_SLOT, view);
}

void Material::SetNormalMap( TextureView *view )
{
	SetTextureView(NORMAL_SLOT, view);
}

void Material::SetSpecularMap( TextureView *view )
{
	SetTextureView(SPECULAR_SLOT, view);
}

void Material::SetEmissiveMap( TextureView *view )
{
	SetTextureView(EMISSIVE_SLOT, view);
}

void Material::SetUniforms( UniformBuffer *ubo )
{
	m_materialBuffer = ubo;
}

void Material::SetUniforms( void const *data, size_t const size )
{
	UniformBuffer* buffer = new UniformBuffer(m_renderContext);
	bool result = buffer->CopyCPUToGPU(data, size);

	if(!result)
	{
		ERROR_AND_DIE("The uniform buffer was not set for Material");
	}
}

UniformBuffer* Material::GetUniformBuffer() const
{
	return m_materialBuffer;
}

