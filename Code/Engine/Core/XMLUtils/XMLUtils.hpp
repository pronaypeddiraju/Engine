//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include <ThirdParty/TinyXML2/tinyxml2.h>
#include <string>

//------------------------------------------------------------------------------------------------------------------------------
typedef tinyxml2::XMLElement XMLElement;

struct Rgba;
struct Vec2;
struct Vec3;
struct IntRange;
struct FloatRange;
struct IntVec2;

//------------------------------------------------------------------------------------------------------------------------------
std::string						ParseXmlAttribute(const XMLElement& xmlElement, const char* attributeName, const std::string& defaultValue);
std::string						ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const char* defaultValue=nullptr );
int								ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, int defaultValue );
char							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, char defaultValue );
bool							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, bool defaultValue );
float							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, float defaultValue );
Rgba							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const Rgba& defaultValue );
Vec2							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const Vec2& defaultValue );
Vec3							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const Vec3& defaultValue );
IntRange						ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const IntRange& defaultValue );
FloatRange						ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const FloatRange& defaultValue );
IntVec2							ParseXmlAttribute( const XMLElement& xmlElement, const char* attributeName, const IntVec2& defaultValue );
uint							ParseXmlAttribute(const XMLElement& xmlElement, const char* attributeName, uint defaultValue);