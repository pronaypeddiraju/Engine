#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Material::Material( RenderContext *renderContext )
{
	m_renderContext = renderContext;
}

Material::Material( RenderContext *renderContext, const std::string& fileName )
{
	m_renderContext = renderContext;
	LoadMaterialFromXML(fileName);

	m_renderContext->m_materialDatabase[fileName] = this;
}

Material::~Material()
{


	delete m_materialBuffer;
	m_materialBuffer = nullptr;
}

void Material::LoadMaterialFromXML( const std::string& fileName )
{
	//Open the xml file and parse it
	tinyxml2::XMLDocument materialDoc;
	materialDoc.LoadFile(fileName.c_str());

	if(materialDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		/*
		printf("\n >> Error loading XML file from %s ", fileName);
		printf("\n >> Error ID : %i ", shaderDoc.ErrorID());
		printf("\n >> Error line number is : %i", shaderDoc.ErrorLineNum());
		printf("\n >> Error name : %s", shaderDoc.ErrorName());
		*/
		ERROR_AND_DIE(">> Error loading Material XML file ");
		return;
	}

	XMLElement* rootElement = materialDoc.RootElement();

	m_materialName = ParseXmlAttribute(*rootElement, "id", m_materialName);
	m_shaderName = ParseXmlAttribute(*rootElement, "shader", m_shaderName);

	XMLElement* child = rootElement->FirstChildElement();
	m_diffuseName = ParseXmlAttribute(*child, "src", m_diffuseName);

	child = child->NextSiblingElement();
	m_normalName = ParseXmlAttribute(*child, "src", m_normalName);

	child = child->NextSiblingElement();
	m_specName = ParseXmlAttribute(*child, "src", m_specName);

	child = child->NextSiblingElement();
	m_samplerIndex = ParseXmlAttribute(*child, "idx", m_samplerIndex);
	m_samplerType = ParseXmlAttribute(*child, "type", m_samplerType);

	Sampler* sampler = m_renderContext->GetSamplerOfType(m_samplerType);
	SetSampler(m_samplerIndex, sampler);

	child = child->NextSiblingElement();
	m_samplerIndex = ParseXmlAttribute(*child, "idx", m_samplerIndex);
	m_samplerType = ParseXmlAttribute(*child, "type", m_samplerType);

	sampler = m_renderContext->GetSamplerOfType(m_samplerType);
	SetSampler(m_samplerIndex, sampler);

	//Now set the required data
	SetShader(m_shaderName.c_str());
	SetTextureView(DIFFUSE_SLOT, m_diffuseName.c_str());
	SetTextureView(NORMAL_SLOT, m_normalName.c_str());
	SetTextureView(SPECULAR_SLOT, m_specName.c_str());

}

void Material::SetShader( Shader *shader )
{
	m_shader = shader;
}

void Material::SetShader( char const *shader_name )
{
	if(strcmp(shader_name, "") == 0)
	{
		ERROR_AND_DIE("The shader was not defined in material XML");
		return;
	}

	//We have valid name
	m_shader = m_renderContext->CreateOrGetShaderFromFile(shader_name);
}

void Material::SetTextureView( uint slot, TextureView *view )
{
	uint numElements = static_cast<uint>(m_textures.size());

	if(numElements <= slot)
	{
		m_textures.resize(slot + 1);
	}

	//Set the texture view at the received slot in vector
	m_textures[slot] = view;
}

void Material::SetTextureView( uint slot, char const *name )
{
	if(strcmp(name, "") == 0)
	{
		ERROR_AND_DIE("The texture was not defined in material XML");
		return;
	}

	//We have a valid name
	TextureView* texture = m_renderContext->GetOrCreateTextureViewFromFile(name);
	SetTextureView(slot, texture);
}

void Material::SetSampler( uint slot, Sampler *sampler )
{
	uint numElements = static_cast<uint>(m_samplers.size());

	if(numElements <= slot)
	{
		m_samplers.resize(slot + 1);
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
	if(view == nullptr)
	{
		SetTextureView(DIFFUSE_SLOT, "WHITE.png");
		return;
	}

	SetTextureView(DIFFUSE_SLOT, view);
}

void Material::SetNormalMap( TextureView *view )
{
	if(view == nullptr)
	{
		SetTextureView(NORMAL_SLOT, "FLAT.png");
		return;
	}

	SetTextureView(NORMAL_SLOT, view);
}

void Material::SetSpecularMap( TextureView *view )
{
	if(view == nullptr)
	{
		SetTextureView(SPECULAR_SLOT, "WHITE.png");
	}

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

int Material::GetNumTextures()
{
	return static_cast<int>(m_textures.size());
}

int Material::GetNumSamplers()
{
	return static_cast<int>(m_samplers.size());
}

