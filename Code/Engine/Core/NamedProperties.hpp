//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <string>
#include <map>
#include <cerrno>
#include <cstdlib>
// Engine Systems
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
// Base property type to allow us to make a good interface for all types by calling a generic type's ToString
//------------------------------------------------------------------------------------------------------------------------------
class BaseProperty
{
public:
	//virtual std::string AsString() const = 0;
	virtual std::string ToString() const = 0;
};

//------------------------------------------------------------------------------------------------------------------------------
// Generic template typedProperty. This is used for type checking using RTTI (Run time type information) or using static memory
// to create a unique identifier for types (This is I will  use because I can pass it void pointers)
//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
class TypedProperty : public BaseProperty
{
public:
	TypedProperty(T const &val)
		: m_value(val)
	{}

	//Using duck typing (This will fail if there is no ToString() specified for the type being passed)
	virtual std::string ToString() const override
	{
		return ::ToString(m_value);
	}

public:
	T m_value;
};

//------------------------------------------------------------------------------------------------------------------------------
class NamedProperties
{
public:
	NamedProperties();
	~NamedProperties();

	//std::string GetPropertyString(std::string const &key, std::string const &def = "");

public:

	//------------------------------------------------------------------------------------------------------------------------------
	// template interface functions
	//------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	void SetValue(std::string const &key, T const &value)
	{
		//calls set value on T
		TypedProperty<T> *prop = new TypedProperty<T>(value);

		std::map<std::string, BaseProperty*>::iterator itr = m_properties.find(key);
		if (itr != m_properties.end())
		{
			delete itr->second;
		}

		m_properties[key] = prop;
	}

	template <typename T>
	T GetValue(std::string const &key, T const &defaultValue)
	{
		std::map<std::string, BaseProperty*>::iterator value = m_properties.find(key);
		if (value == m_properties.end()) 
		{
			return defaultValue;
		}

		BaseProperty *prop = value->second;

		TypedProperty<T> *typedProp = dynamic_cast<TypedProperty<T>*>(prop);
		if (typedProp == nullptr) 
		{
			std::string str = prop->ToString();
			return FromString(str, defaultValue);
		}
		else {
			return typedProp->m_value;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------
	//	Handling cases where pointers are passed to the GetValue and SetValue functions
	//------------------------------------------------------------------------------------------------------------------------------
	template <typename T>
	T GetValue(std::string const &key, T *def)
	{
		std::map<std::string, BaseProperty*>::iterator value;
		value = m_properties.find(key);
		
		if (value == m_properties.end()) 
		{
			return def;
		}

		BaseProperty *prop = value->second;
		TypedProperty<T*> *typedProp = dynamic_cast<TypedProperty<T*>*>(prop);
		if (typedProp == nullptr) 
		{
			return def;
		}
		else {
			return typedProp->m_value;
		}
	}

	template <typename T>
	T SetValue(std::string const &key, T *ptr)
	{
		//Dereference and call on type T
		SetValue<T*>(key, ptr);
	}

	// Specializations
	void				SetValue(std::string const &key, char const* defaultValue);
	std::string			GetValue(std::string const &key, char const* defaultValue) const;
	std::string			GetValue(const std::string& key, std::string defaultValue) const;
	bool				GetValue(const std::string& key, bool defaultValue) const;
	int					GetValue(const std::string& key, int defaultValue) const;
	float				GetValue(const std::string& key, float defaultValue) const;

private:
	std::map<std::string, BaseProperty*> m_properties;
};

inline std::string ToString(void const * ptr) { UNUSED(ptr);  return ""; }

//------------------------------------------------------------------------------------------------------------------------------
// Template functions to get from string
//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T FromString(char const *str, T const &def)
{
	return T(str);
}

template <>
float FromString(char const *str, float const &def);

template <>
bool FromString(char const* str, bool const &def);

template <>
int FromString(char const* str, int const &def);

template <>
std::string FromString(char const* str, std::string const &def);

//------------------------------------------------------------------------------------------------------------------------------
// Template functions to convert back to string
//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::string ToString( const T &value )
{
	return value.GetAsString();
}

template <>
std::string ToString(float const &value);

template <>
std::string ToString(bool const &value);

template <>
std::string ToString(int const &value);

template <>
std::string ToString(std::string const &value);