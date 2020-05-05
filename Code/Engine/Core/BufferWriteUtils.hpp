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
// Purpose: This class will take some types as input
// It will convert type information to bytes and write to a buffer
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
class BufferWriteUtils
{
public:
	BufferWriteUtils(Buffer& buffer, eBufferEndianness endianMode = eBufferEndianness::BUFFER_NATIVE);

	// Endian-related settings; if ignored, assumes build platform's native endian (LITTLE for x86/x64)
	void					SetEndianMode(eBufferEndianness endianModeToUseForSubsequentReads);
	bool					IsEndianModeBig() const { return (m_endianMode == eBufferEndianness::BUFFER_BIG_ENDIAN) || (m_endianMode == eBufferEndianness::BUFFER_NATIVE && PLATFORM_IS_BIG_ENDIAN); }
	bool					IsEndianModeOppositeNative() const { return m_isOppositeEndian; }

	Buffer&					GetBuffer() const { return m_buffer; }
	size_t					GetTotalSize() const { return m_buffer.size(); }
	size_t					GetAppendedSize() const { return m_buffer.size() - m_initialSize; }
	void					ReserveAdditional(size_t additionalBytes);					// stretch capacity +N bytes beyond current *size*

	// Each of these appends to the buffer's end
	void					AppendByte(uchar paramUchar);
	void					AppendChar(char paramChar);
	void					AppendShort(short paramShort);
	void					AppendInt32(int paramInt);
	void					AppendUint32(uint paramUint);
	void					AppendFloat(float paramFloat);
	void					AppendDouble(double paramDouble);
	void					AppendBool(bool paramBool);									// writes as byte 0x00 for false, 0x01 for true
	void					AppendByteArray(const uchar* byteArray, size_t numBytesToWrite);
	void					AppendByteArray(const std::vector<uchar>& byteArray);
	void					AppendStringZeroTerminated(const char* paramString);
	void					AppendStringZeroTerminated(const std::string& paramString);
	void					AppendStringAfter32BitLength(const char* paramString, uint knownLength = 0);
	void					AppendStringAfter32BitLength(const std::string& paramString);
	void					AppendStringAfter8BitLength(const char* paramString, uchar knownLength = 0);
	void					AppendStringAfter8BitLength(const std::string& paramString);
	void					AppendRgb(const Rgba& rgb_ignoreAlpha);						// writes only 3 bytes (R,G,B) and ignores Alpha
	void					AppendRgba(const Rgba& rgba);								// writes 4 bytes, always in RGBA order
	void					AppendVec2(const Vec2& vec2);								// writes 2 floats: x,y
	void					AppendVec3(const Vec3& vec3);								// writes 3 floats: x,y,z
	void					AppendIntVec2(const IntVec2& ivec2);						// writes 2 ints: x,y
	void					AppendVertexPCU(const Vertex_PCU& vertex);					// writes Vec3 pos, Rgba color, Vec2 uv
	void					AppendVertexMaster(const VertexMaster& vertex);					// writes Vec3 pos, Vec3 normal, Vec3 tangent, Vec3 bitangent, Rgba color, Vec2 uv
	void					AppendZeros(size_t howManyBytesWorthOfZerosToAppend);
	uchar*					AppendUninitializedBytes(size_t howManyJunkBytesToAppend); // returns the START of the new uninitialized bytes (typically to copy into)

	//Writing at a specific location
	void					WriteUint32AtLocation(int writeLocation, uint paramUint);

	eBufferEndianness		m_endianMode = eBufferEndianness::BUFFER_NATIVE;
	bool					m_isOppositeEndian = false;									// false if buffer matches native (don't need to reverse)
	Buffer&					m_buffer;
	size_t					m_initialSize = 0;											// # of bytes in buffer BEFORE this session began appending
};
