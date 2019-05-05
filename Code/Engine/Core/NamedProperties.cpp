//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/NamedProperties.hpp"

//------------------------------------------------------------------------------------------------------------------------------
NamedProperties::NamedProperties()
{

}

//------------------------------------------------------------------------------------------------------------------------------
NamedProperties::~NamedProperties()
{

}

//------------------------------------------------------------------------------------------------------------------------------
template <>
float FromString(char const *str, float const &def)
{
	char * e;
	errno = 0;
	float x = std::strtof(str, &e);

	if (*e != '\0' ||  // error, we didn't consume the entire string
		errno != 0)   // error, overflow or underflow
	{
		// Failed cast :<
		return def;
	}
	else
	{
		//Great success!! :>
		return x;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
bool FromString(char const* str, bool const &def)
{
	if (strcmp(str, "True") == 0)
	{
		return true;
	}
	else if (strcmp(str, "False") == 0)
	{
		return false;
	}
	else
	{
		return def;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
int FromString(char const* str, int const &def)
{
	int x = atoi(str);
	if (x == 0 && str[0] != '0')
	{
		return def;
	}
	else
	{
		return x;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
std::string FromString(char const* str, std::string const &def)
{
	if (str != nullptr)
	{
		return std::string(str);
	}
	else
	{
		return def;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
std::string ToString(float const &value)
{
	return std::to_string(value);
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
std::string ToString(bool const &value)
{
	if (value)
	{
		return "True";
	}
	else
	{
		return "False";
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
std::string ToString(int const &value)
{
	return std::to_string(value);
}

//------------------------------------------------------------------------------------------------------------------------------
template <>
std::string ToString(std::string const &value)
{
	return value;
}
