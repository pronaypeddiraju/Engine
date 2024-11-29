#include "Engine/Core/BufferWriteUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
BufferWriteUtils::BufferWriteUtils(Buffer& buffer, eBufferEndianness endianMode /*= eBufferEndianness::BUFFER_NATIVE*/)
	:	m_buffer(buffer)
{
	SetEndianMode(endianMode);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::SetEndianMode(eBufferEndianness endianModeToUseForSubsequentReads)
{
	m_endianMode = endianModeToUseForSubsequentReads;
	if (m_endianMode == eBufferEndianness::BUFFER_NATIVE || m_endianMode == BUFFER_LITTLE_ENDIAN)
	{
		//No need to opposite endian
		m_isOppositeEndian = false;
	}
	else
	{
		//We need to save in opposite endian
		m_isOppositeEndian = true;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::ReserveAdditional(size_t additionalBytes)
{
	m_buffer.reserve(m_buffer.size() + additionalBytes);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendByte(uchar paramUchar)
{
	ReserveAdditional(sizeof(uchar));

	m_buffer.push_back(paramUchar);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendChar(char paramChar)
{
	ReserveAdditional(sizeof(uchar));

	m_buffer.push_back((uchar)paramChar);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendShort(short paramShort)
{
	uchar asBytes[sizeof(short)];
	std::copy(static_cast<const uchar*>(static_cast<const void*>(&paramShort)),
		static_cast<const uchar*>(static_cast<const void*>(&paramShort)) + sizeof(short),
		asBytes);

	if (IsEndianModeOppositeNative())
	{
		Reverse2BytesInPlace(&asBytes);
	}

	ReserveAdditional(sizeof(short));

	m_buffer.push_back(asBytes[0]);
	m_buffer.push_back(asBytes[1]);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendInt32(int paramInt)
{
	uchar asBytes[sizeof(int)];
	std::copy(static_cast<const uchar*>(static_cast<const void*>(&paramInt)),
		static_cast<const uchar*>(static_cast<const void*>(&paramInt)) + sizeof(int),
		asBytes);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&asBytes);
	}

	ReserveAdditional(sizeof(int));

	m_buffer.push_back(asBytes[0]);
	m_buffer.push_back(asBytes[1]);
	m_buffer.push_back(asBytes[2]);
	m_buffer.push_back(asBytes[3]);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendUint32(uint paramUint)
{
	uchar asBytes[sizeof(uint)];
	std::copy(static_cast<const uchar*>(static_cast<const void*>(&paramUint)),
		static_cast<const uchar*>(static_cast<const void*>(&paramUint)) + sizeof(uint),
		asBytes);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&asBytes);
	}

	ReserveAdditional(sizeof(uint));

	m_buffer.push_back(asBytes[0]);
	m_buffer.push_back(asBytes[1]);
	m_buffer.push_back(asBytes[2]);
	m_buffer.push_back(asBytes[3]);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendFloat(float paramFloat)
{
	uchar asBytes[sizeof(float)];
	std::copy(static_cast<const uchar*>(static_cast<const void*>(&paramFloat)),
		static_cast<const uchar*>(static_cast<const void*>(&paramFloat)) + sizeof(float),
		asBytes);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&asBytes);
	}

	ReserveAdditional(sizeof(float));

	m_buffer.push_back(asBytes[0]);
	m_buffer.push_back(asBytes[1]);
	m_buffer.push_back(asBytes[2]);
	m_buffer.push_back(asBytes[3]);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendDouble(double paramDouble)
{
	uchar asBytes[sizeof(double)];
	std::copy(static_cast<const uchar*>(static_cast<const void*>(&paramDouble)),
		static_cast<const uchar*>(static_cast<const void*>(&paramDouble)) + sizeof(double),
		asBytes);

	if (IsEndianModeOppositeNative())
	{
		Reverse8BytesInPlace(&asBytes);
	}

	ReserveAdditional(sizeof(double));

	m_buffer.push_back(asBytes[0]);
	m_buffer.push_back(asBytes[1]);
	m_buffer.push_back(asBytes[2]);
	m_buffer.push_back(asBytes[3]);
	m_buffer.push_back(asBytes[4]);
	m_buffer.push_back(asBytes[5]);
	m_buffer.push_back(asBytes[6]);
	m_buffer.push_back(asBytes[7]);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendBool(bool paramBool)
{
	if (paramBool)
	{
		AppendByte(1);
	}
	else
	{
		AppendByte(0);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendByteArray(const uchar* byteArray, size_t numBytesToWrite)
{	
	for (uint byteIndex = 0; byteIndex < (uint)numBytesToWrite; byteIndex++)
	{
		AppendByte(*byteArray);
		byteArray++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendByteArray(const std::vector<uchar>& byteArray)
{
	std::vector<uchar>::const_iterator byteItr = byteArray.begin();

	while (byteItr != byteArray.end())
	{
		AppendByte(*byteItr);

		byteItr++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendStringZeroTerminated(const char* paramString)
{
	int stringLength = (int)strlen(paramString);
	
	const char* ch = paramString;

	for (int i = 0; i < stringLength; i++)
	{
		AppendChar(*ch);
		ch++;
	}

	AppendChar('\0');
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendStringZeroTerminated(const std::string& paramString)
{
	const char* ch = &paramString[0];
	while (*ch != '\0')
	{
		AppendChar(*ch);
		ch++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendStringAfter32BitLength(const char* paramString, uint knownLength /*= 0*/)
{
	int stringLength = (int)strlen(paramString);

	if (knownLength != (uint)stringLength)
	{
		knownLength = stringLength;
	}

	//First write the length into the buffer
	AppendUint32(knownLength);
	
	const char* ch = paramString;

	for (int i = 0; i < stringLength; i++)
	{
		AppendChar(*ch);
		ch++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendStringAfter32BitLength(const std::string& paramString)
{
	//First write the length into the buffer
	AppendUint32((uint)paramString.length());
	
	const char* ch = &paramString[0];

	for (int i = 0; i < paramString.length(); i++)
	{
		AppendChar(*ch);
		ch++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendStringAfter8BitLength(const char* paramString, uchar knownLength /*= 0*/)
{
	int stringLength = (int)strlen(paramString);

	if (knownLength != (uchar)stringLength)
	{
		knownLength = (uchar)stringLength;
	}

	//First write length as 8 bit value
	AppendByte(knownLength);
	AppendByteArray(reinterpret_cast<const uchar*>(paramString), knownLength);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendStringAfter8BitLength(const std::string& paramString)
{
	//First write length as 8 bit value
	AppendByte((uchar)paramString.length());
	AppendByteArray(reinterpret_cast<const uchar*>(paramString.c_str()), paramString.length());
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendRgb(const Rgba& rgb_ignoreAlpha)
{
	AppendByte((uchar)(rgb_ignoreAlpha.r * 255.f));
	AppendByte((uchar)(rgb_ignoreAlpha.g * 255.f));
	AppendByte((uchar)(rgb_ignoreAlpha.b * 255.f));
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendRgba(const Rgba& rgba)
{
	AppendByte((uchar)(rgba.r * 255.f));
	AppendByte((uchar)(rgba.g * 255.f));
	AppendByte((uchar)(rgba.b * 255.f));
	AppendByte((uchar)(rgba.a * 255.f));
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendVec2(const Vec2& vec2)
{
	AppendFloat(vec2.x);
	AppendFloat(vec2.y);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendVec3(const Vec3& vec3)
{
	AppendFloat(vec3.x);
	AppendFloat(vec3.y);
	AppendFloat(vec3.z);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendIntVec2(const IntVec2& ivec2)
{
	AppendInt32(ivec2.x);
	AppendInt32(ivec2.y);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendVertexPCU(const Vertex_PCU& vertex)
{
	AppendVec3(vertex.m_position);
	AppendRgba(vertex.m_color);
	AppendVec2(vertex.m_uvTexCoords);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendVertexMaster(const VertexMaster& vertex)
{
	AppendVec3(vertex.m_position);
	AppendVec3(vertex.m_normal);
	AppendVec3(vertex.m_tangent);
	AppendVec3(vertex.m_biTangent);
	AppendRgba(vertex.m_color);
	AppendVec2(vertex.m_uv);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::AppendZeros(size_t howManyBytesWorthOfZerosToAppend)
{
	uchar ch = 0;

	for (int i = 0; i < howManyBytesWorthOfZerosToAppend; i++)
	{
		AppendByte(ch);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
uchar* BufferWriteUtils::AppendUninitializedBytes(size_t howManyJunkBytesToAppend)
{
	uchar* locationOfJunk = &m_buffer[m_buffer.size() - 1];
	uchar junk = 0;

	for (int i = 0; i < howManyJunkBytesToAppend; i++)
	{
		AppendByte(junk);
	}

	return locationOfJunk;
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferWriteUtils::WriteUint32AtLocation(int writeLocation, uint paramUint)
{
	uchar asBytes[sizeof(uint)];
	std::copy(static_cast<const uchar*>(static_cast<const void*>(&paramUint)),
		static_cast<const uchar*>(static_cast<const void*>(&paramUint)) + sizeof(uint),
		asBytes);

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&asBytes);
	}

	m_buffer[writeLocation] = asBytes[0];
	m_buffer[writeLocation + 1] = asBytes[1];
	m_buffer[writeLocation + 2] = asBytes[2];
	m_buffer[writeLocation + 3] = asBytes[3];
}
