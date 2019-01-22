#pragma once
#include <string>
#include <vector>

class Tags
{
public:

	explicit Tags(const std::string& commaSeparatedTagNames);
	explicit Tags();


	// These support comma-separated strings of 0+ tag names, with “!” (NOT) tag names to remove
	void SetOrRemoveTags( const std::string& commaSeparatedTagNames ); // "blessed,!poisoned"
	bool HasTags( const std::string& commaSeparatedTagNames ); // "good,!cursed"

	// These each take exactly one tag name, and do not support commas nor “!” (NOT) tags
	void SetTag( const std::string& tagName );
	void RemoveTag( const std::string& tagName );
	bool HasTag( const std::string& tagName );

	//Get all strings in m_tags
	const std::string	GetAllTags() const;

protected:
	std::vector<std::string> m_tags;
};
