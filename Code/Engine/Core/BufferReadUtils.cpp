#include "Engine/Core/BufferReadUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
BufferReadUtils::BufferReadUtils(const unsigned char* bufferData, size_t bufferSize, eBufferEndianness endianMode /*= eBufferEndianness::BUFFER_NATIVE*/)
{
	m_bufferSize = bufferSize;
	m_scanStart = &bufferData[0];
	m_scanPosition = &bufferData[0];
	m_scanEnd = &bufferData[bufferSize - 1];
	SetEndianMode(endianMode);
}

//------------------------------------------------------------------------------------------------------------------------------
BufferReadUtils::BufferReadUtils(const Buffer& buffer, eBufferEndianness endianMode /*= eBufferEndianness::BUFFER_NATIVE*/)
{
	m_bufferSize = buffer.size();
	m_scanStart = &buffer[0];
	m_scanPosition = &buffer[0];
	m_scanEnd = &buffer[m_bufferSize - 1];
	SetEndianMode(endianMode);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::SetEndianMode(eBufferEndianness endianModeToUseForSubsequentReads)
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
uchar BufferReadUtils::ParseByte()
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(uchar));

	//Read 1 uchar from the buffer
	uchar parsedValue;
	parsedValue = (uchar)*m_scanPosition;
	(uchar)m_scanPosition++;
	return parsedValue;
}

//------------------------------------------------------------------------------------------------------------------------------
char BufferReadUtils::ParseChar()
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(char));

	char parsedValue;
	parsedValue = (char)*m_scanPosition;
	(uchar)m_scanPosition++;
	return parsedValue;
}

//------------------------------------------------------------------------------------------------------------------------------
int BufferReadUtils::ParseInt32()
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(int));
	int parsedValue = 0;

	uchar bytes[4];
	bytes[0] = *m_scanPosition;
	m_scanPosition++;
	bytes[1] = *m_scanPosition;
	m_scanPosition++;
	bytes[2] = *m_scanPosition;
	m_scanPosition++;
	bytes[3] = *m_scanPosition;
	m_scanPosition++;

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&bytes);
	}

	memcpy(&parsedValue, &bytes, sizeof(int));

	return parsedValue;
}

//------------------------------------------------------------------------------------------------------------------------------
uint BufferReadUtils::ParseUint32()
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(uint));

	uint parsedValue = 0;

	uchar bytes[4];
	bytes[0] = *m_scanPosition;
	m_scanPosition++;
	bytes[1] = *m_scanPosition;
	m_scanPosition++;
	bytes[2] = *m_scanPosition;
	m_scanPosition++;
	bytes[3] = *m_scanPosition;
	m_scanPosition++;

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&bytes);
	}

	memcpy(&parsedValue, &bytes, sizeof(uint));

	return parsedValue;
}

//------------------------------------------------------------------------------------------------------------------------------
float BufferReadUtils::ParseFloat()
{
	uchar bytes[4];
	float parsedValue;

	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(float));

	bytes[0] = *m_scanPosition;
	m_scanPosition++;
	bytes[1] = *m_scanPosition;
	m_scanPosition++;
	bytes[2] = *m_scanPosition;
	m_scanPosition++;
	bytes[3] = *m_scanPosition;
	m_scanPosition++;

	if (IsEndianModeOppositeNative())
	{
		Reverse4BytesInPlace(&bytes);
	}

	memcpy(&parsedValue, &bytes, sizeof(float));
	return parsedValue;
}

//------------------------------------------------------------------------------------------------------------------------------
double BufferReadUtils::ParseDouble()
{
	uchar bytes[8];
	double parsedValue;

	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(double));

	//First 4 bytes
	bytes[0] = *m_scanPosition;
	m_scanPosition++;
	bytes[1] = *m_scanPosition;
	m_scanPosition++;
	bytes[2] = *m_scanPosition;
	m_scanPosition++;
	bytes[3] = *m_scanPosition;
	m_scanPosition++;

	//next 4 bytes
	bytes[4] = *m_scanPosition;
	m_scanPosition++;
	bytes[5] = *m_scanPosition;
	m_scanPosition++;
	bytes[6] = *m_scanPosition;
	m_scanPosition++;
	bytes[7] = *m_scanPosition;
	m_scanPosition++;

	if (IsEndianModeOppositeNative())
	{
		Reverse8BytesInPlace(&bytes);
	}

	memcpy(&parsedValue, &bytes, sizeof(double));
	return parsedValue;
}

//------------------------------------------------------------------------------------------------------------------------------
bool BufferReadUtils::ParseBool()
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(uchar));

	uchar parsedValue = (uchar)*m_scanPosition;
	m_scanPosition++;
	if (parsedValue == 0)
	{
		//False byte
		return false;
	}
	else
	{
		//True byte
		return true;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
const uchar* BufferReadUtils::ParseBytes(size_t numBytes)
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(numBytes);

	uchar* byteArray = new uchar[numBytes];

	for (int byteIndex = 0; byteIndex < (int)numBytes; byteIndex++)
	{
		byteArray[byteIndex] = *m_scanPosition;
		m_scanPosition++;
	}

	return byteArray;
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::ParseByteArray(std::vector<uchar>& out_bytes, size_t numBytesToParse)
{
	//Parse numBytesToParse from the current scan position
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(numBytesToParse);

	//First resize the vector to ensure we have the required space
	out_bytes.resize(numBytesToParse);

	for (uint byteIndex = 0; byteIndex < numBytesToParse; byteIndex++)
	{
		out_bytes[byteIndex] = *m_scanPosition;
		m_scanPosition++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::ParseByteArray(uchar* out_destArray, size_t numBytesToParse)
{
	//We are going to assume the buffer is of the right size
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(numBytesToParse);

	for (int byteIndex = 0; byteIndex < (int)numBytesToParse; byteIndex++)
	{
		out_destArray[byteIndex] = *m_scanPosition;
		m_scanPosition++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::ParseStringZeroTerminated(std::string& out_string)
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(uchar));

	uchar byte = *m_scanPosition;
	while (byte != '\0')
	{
		out_string += byte;

		//Check if we have the correct number of bytes in the buffer remaining
		GuaranteeBufferDataAvailable(sizeof(uchar));

		m_scanPosition++;
		byte = *m_scanPosition;
	}

	m_scanPosition++;
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::ParseStringAfter32BitLength(std::string& out_string)
{
	//Check if we have the correct number of bytes in the buffer remaining
	GuaranteeBufferDataAvailable(sizeof(uint));

	int stringlength = ParseInt32();

	out_string.resize(stringlength);
	ParseByteArray((uchar*)&out_string[0], stringlength);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::ParseStringAfter8BitLength(std::string& out_string)
{
	GuaranteeBufferDataAvailable(sizeof(uchar));

	uchar stringlength = ParseByte();

	ParseByteArray((uchar*)&out_string[0], stringlength);
}

//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::ParseStringOfLength(std::string& out_string, uint stringLength)
{
	GuaranteeBufferDataAvailable(stringLength);

	ParseByteArray((uchar*)&out_string[0], stringLength);
}

//------------------------------------------------------------------------------------------------------------------------------
Rgba BufferReadUtils::ParseRgb()
{
	uchar rByte = ParseByte();
	uchar gByte = ParseByte();
	uchar bByte = ParseByte();
	uchar aByte = 255;

	Rgba color;
	color.SetFromBytes(rByte, gByte, bByte, aByte);
	
	return color;
}

//------------------------------------------------------------------------------------------------------------------------------
Rgba BufferReadUtils::ParseRgba()
{
	uchar rByte = ParseByte();
	uchar gByte = ParseByte();
	uchar bByte = ParseByte();
	uchar aByte = ParseByte();

	Rgba color;
	color.SetFromBytes(rByte, gByte, bByte, aByte);

	return color;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 BufferReadUtils::ParseVec2()
{
	Vec2 vector;
	vector.x = ParseFloat();
	vector.y = ParseFloat();

	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3 BufferReadUtils::ParseVec3()
{
	Vec3 vector;
	vector.x = ParseFloat();
	vector.y = ParseFloat();
	vector.z = ParseFloat();

	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2 BufferReadUtils::ParseIntVec2()
{
	IntVec2 intVec;
	intVec.x = ParseInt32();
	intVec.y = ParseInt32();

	return intVec;
}

//------------------------------------------------------------------------------------------------------------------------------
Vertex_PCU BufferReadUtils::ParseVertexPCU()
{
	Vertex_PCU vertex;
	vertex.m_position = ParseVec3();
	vertex.m_color = ParseRgba();
	vertex.m_uvTexCoords = ParseVec2();

	return vertex;
}

//------------------------------------------------------------------------------------------------------------------------------
VertexMaster BufferReadUtils::ParseVertexMaster()
{
	VertexMaster vertex;
	vertex.m_position = ParseVec3();
	vertex.m_normal = ParseVec3();
	vertex.m_tangent = ParseVec3();
	vertex.m_biTangent = ParseVec3();

	vertex.m_color = ParseRgba();
	vertex.m_uv = ParseVec2();

	return vertex;
}

//------------------------------------------------------------------------------------------------------------------------------
bool BufferReadUtils::IsBufferDataAvailable(size_t numBytes) const
{
	return m_scanPosition + (numBytes - 1) <= m_scanEnd;
}


//------------------------------------------------------------------------------------------------------------------------------
void BufferReadUtils::GuaranteeBufferDataAvailable(size_t numBytes) const
{
	GUARANTEE_OR_DIE(m_scanPosition + (numBytes - 1) <= m_scanEnd, Stringf("Insufficient data in buffer to read %i bytes", numBytes));
}
