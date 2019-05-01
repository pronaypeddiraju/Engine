//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
NamedStrings::NamedStrings()
{

}

//------------------------------------------------------------------------------------------------------------------------------
NamedStrings::NamedStrings( const std::string& keyName, const std::string& value )
{
	m_keyValuePairs[keyName] = value;
}

//------------------------------------------------------------------------------------------------------------------------------
NamedStrings::~NamedStrings()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void NamedStrings::PopulateFromXmlElementAttributes( const XMLElement& element )
{
	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute; attribute = attribute->Next())
	{
		m_keyValuePairs[attribute->Name()] = attribute->Value();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

//------------------------------------------------------------------------------------------------------------------------------
bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		if(stringPair->second == "true" || stringPair->second == "True")
			return true;
		else
			return false;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		return atoi(stringPair->second.c_str());
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		return static_cast<float>(atof(stringPair->second.c_str()));
	}
	else
		return defaultValue;
}

//------------------------------------------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		return stringPair->second;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		return stringPair->second;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Rgba NamedStrings::GetValue( const std::string& keyName, const Rgba& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		Rgba newRgba = Rgba(stringPair->second.c_str());
		return newRgba;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		Vec2 newVec2 = Vec2(stringPair->second.c_str());
		return newVec2;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		IntVec2 newIntVec2 = IntVec2(stringPair->second.c_str());
		return newIntVec2;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
FloatRange NamedStrings::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		FloatRange newFloatRange = FloatRange(stringPair->second.c_str());
		return newFloatRange;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
IntRange NamedStrings::GetValue( const std::string& keyName, const IntRange& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator stringPair = m_keyValuePairs.find(keyName);
	if(stringPair != m_keyValuePairs.end())
	{
		IntRange newIntRange = IntRange(stringPair->second.c_str());
		return newIntRange;
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
size_t NamedStrings::GetNamedStringSize() const
{
	return m_keyValuePairs.size();
}
