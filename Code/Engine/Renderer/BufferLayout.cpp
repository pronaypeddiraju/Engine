#include "Engine/Renderer/BufferLayout.hpp"

//------------------------------------------------------------------------------------------------------------------------------
BufferAttributeT::BufferAttributeT( const std::string& attributeName, eDataFormat type, const size_t& size )
{
	m_name = attributeName;
	m_type = type;
	m_memberOffset = size;
}

//------------------------------------------------------------------------------------------------------------------------------
BufferAttributeT::BufferAttributeT()
{

}

//------------------------------------------------------------------------------------------------------------------------------
BufferAttributeT::~BufferAttributeT()
{

}

//------------------------------------------------------------------------------------------------------------------------------
BufferLayout::BufferLayout()
{

}

//------------------------------------------------------------------------------------------------------------------------------
const BufferLayout* BufferLayout::For( BufferAttributeT const *attributeList, size_t stride, CopyFromMasterCallback copyCallback )
{
	//This function is called by the template and creates bufferLayout for any Vertex type using the LAYOUT of that Vertex type
	BufferLayout* bufferLayout = new BufferLayout;
	
	int i = 0;
	while(!attributeList[i].IsNull())
	{
		bufferLayout->m_attributes.push_back(attributeList[i]);
		i++;
	}
	
	bufferLayout->m_stride = static_cast<uint>(stride);
	bufferLayout->m_copyFromMaster = copyCallback;

	return bufferLayout;
}
