//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"
#include <map>
#include <string>

struct Rgba;
struct IntVec2;
struct FloatRange;
struct IntRange;

//------------------------------------------------------------------------------------------------------------------------------
class NamedStrings
{
public:
	NamedStrings();
	explicit NamedStrings(const std::string& keyName, const std::string& value);
	~NamedStrings();

	void				PopulateFromXmlElementAttributes( const XMLElement& element );
	void				SetValue( const std::string& keyName, const std::string& newValue );

	bool				GetValue( const std::string& keyName, bool defaultValue ) const;
	int					GetValue( const std::string& keyName, int defaultValue ) const;
	float				GetValue( const std::string& keyName, float defaultValue ) const;
	std::string			GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string			GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba				GetValue( const std::string& keyName, const Rgba& defaultValue ) const;
	Vec2				GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
	IntVec2				GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;
	FloatRange			GetValue( const std::string& keyName, const FloatRange& defaultValue ) const;
	IntRange			GetValue( const std::string& keyName, const IntRange& defaultValue ) const;

	size_t				GetNamedStringSize() const;

private:
	std::map< std::string, std::string >	m_keyValuePairs;
};