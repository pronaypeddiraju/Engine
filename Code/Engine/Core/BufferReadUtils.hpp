#pragma once
#include "Engine/Core/BufferUtilCommons.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Math/VertexMaster.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// Purpose: This class will take a buffer as input
// It will parse the buffer and determine what data types exist in it
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
class BufferReadUtils
{
public:
	BufferReadUtils(const unsigned char* bufferData, size_t bufferSize, eBufferEndianness endianMode = eBufferEndianness::BUFFER_NATIVE);
	BufferReadUtils(const Buffer& buffer, eBufferEndianness endianMode = eBufferEndianness::BUFFER_NATIVE);

	void					SetEndianMode(eBufferEndianness endianModeToUseForSubsequentReads);
	bool					IsEndianModeBig() const { return (m_endianMode == eBufferEndianness::BUFFER_BIG_ENDIAN) || (m_endianMode == eBufferEndianness::BUFFER_NATIVE && PLATFORM_IS_BIG_ENDIAN); }
	bool					IsEndianModeOppositeNative() const { return m_isOppositeEndian; }

	bool					SetReadLocation(uint newScanLocation);

	uchar					ParseByte();
	char					ParseChar();
	short					ParseShort();
	int						ParseInt32();
	uint					ParseUint32();
	float					ParseFloat();
	double					ParseDouble();
	bool					ParseBool();
	const uchar*			ParseBytes(size_t numBytes);
	void					ParseByteArray(std::vector<uchar>& out_bytes, size_t numBytesToParse);
	void					ParseByteArray(uchar* out_destArray, size_t numBytesToParse);
	void					ParseStringZeroTerminated(std::string& out_string);
	void					ParseStringAfter32BitLength(std::string& out_string);
	void					ParseStringAfter8BitLength(std::string& out_string);
	void					ParseStringOfLength(std::string& out_string, uint stringLength);
	Rgba					ParseRgb();																// reads only 3 bytes (R,G,B) and assumes Alpha=255
	Rgba					ParseRgba();															// reads 4 bytes in RGBA order
	Vec2					ParseVec2();															// reads 2 floats: x,y
	Vec3					ParseVec3();															// reads 3 floats: x,y,z
	IntVec2					ParseIntVec2();															// reads 2 ints: x,y
	Vertex_PCU				ParseVertexPCU();														// reads Vec3 pos, Rgba color, Vec2 uv
	VertexMaster			ParseVertexMaster();														// reads Vec3 pos, Vec3 normal, Vec3 tangent, Vec3 bitangent, Rgba color, Vec2 uv

	inline size_t			GetTotalSize() const { return m_bufferSize; }
	inline size_t			GetRemainingSize() const { return (m_scanEnd - m_scanPosition) + 1; }
	inline bool				IsAtEnd() const { return m_scanPosition > m_scanEnd; }
	bool					IsBufferDataAvailable(size_t numBytes) const;							// return false if not
	void					GuaranteeBufferDataAvailable(size_t numBytes) const;					// error/assert if not!

private:
	eBufferEndianness		m_endianMode = eBufferEndianness::BUFFER_NATIVE;
	bool					m_isOppositeEndian = false;
	size_t					m_bufferSize = 0;
	const uchar*			m_scanStart = nullptr; // start of buffer data being parsed
	const uchar*			m_scanPosition = nullptr; // moves from start to end+1
	const uchar*			m_scanEnd = nullptr; // last VALID character in buffer
};