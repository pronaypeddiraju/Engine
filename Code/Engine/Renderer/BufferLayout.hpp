#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include <string.h>

//------------------------------------------------------------------------------------------------------------------------------
struct VertexMaster;

//------------------------------------------------------------------------------------------------------------------------------
typedef void (*CopyFromMasterCallback)( void *dst, VertexMaster const *src, uint count ); 

//------------------------------------------------------------------------------------------------------------------------------
struct BufferAttributeT 
{
	explicit BufferAttributeT( const std::string& attributeName, eDataFormat type, const size_t& size);
	BufferAttributeT();
	~BufferAttributeT();

	inline bool IsNull() const   { return m_type == DF_NULL; }

	std::string m_name;       // what is the semantic name 
	eDataFormat m_type = DF_NULL;         // what is the type
	size_t m_memberOffset;   // where does this attribute start
};

class BufferLayout
{
public:
	// common constructors
	BufferLayout(); 

	inline uint GetAttributeCount() const		{ return static_cast<uint>(m_attributes.size()); }
	inline uint GetStride() const				{ return m_stride; }

public:
	std::vector<BufferAttributeT> m_attributes;   // what is in this buffer and how does it bind
	uint m_stride;                                  // how large is a single element
	CopyFromMasterCallback m_copyFromMaster;        // how do we copy master to this format?

	
	// This static function is called by the template to create a BufferLayout for any type of Vertex that is passed to it
	// Static ways to look for a unique layout pointer based an attribute list; 
	static const BufferLayout* For( BufferAttributeT const *attributeList, size_t stride, CopyFromMasterCallback copyCallback ); 

	// template helper
	// usage: BufferLayout const* layout = BufferLayout::For<VertexPCU>(); 
	// NOTE: This template assumes that each vertex type has a CopyFromMaster and a LAYOUT defined with it
	template <typename T>
	static const BufferLayout* For() { return For( T::LAYOUT, sizeof(T), T::CopyFromMaster ); } 
};