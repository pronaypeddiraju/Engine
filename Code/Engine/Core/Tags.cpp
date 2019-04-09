//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/StringUtils.hpp"
#include "Engine/Core/Tags.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Tags::Tags()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Tags::Tags(const std::string& commaSeparatedTagNames)
{
	SetOrRemoveTags(commaSeparatedTagNames);
}

//------------------------------------------------------------------------------------------------------------------------------
void Tags::SetOrRemoveTags( const std::string& commaSeparatedTagNames )
{
	//Get split strings
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commaSeparatedTagNames, ',');

	for(int stringIndex = 0; stringIndex < static_cast<int>(splitStrings.size()); stringIndex++)
	{
		//Check if the first char is a ! and call Remove tag
		if(splitStrings[stringIndex][0] == '!')
		{
			RemoveTag(splitStrings[stringIndex]);
			continue;
		}
		else
		{
			//Else add tag
			SetTag(splitStrings[stringIndex]);
			continue;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool Tags::HasTags( const std::string& commaSeparatedTagNames )
{
	//Get split strings
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commaSeparatedTagNames, ',');
	bool hasTags = false;

	for(int stringIndex = 0; stringIndex < static_cast<int>(splitStrings.size()); stringIndex++)
	{
		if(HasTag(splitStrings[stringIndex]))
		{
			hasTags = true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Tags::SetTag( const std::string& tagName )
{
	std::vector<std::string>::iterator tagIterator;

	tagIterator = find(m_tags.begin(), m_tags.end(), tagName);
	if(tagIterator == m_tags.end())
	{
		m_tags.push_back(tagName);
	}
	else
	{
		//Tag already exists
		return;
	}
	
}

//------------------------------------------------------------------------------------------------------------------------------
void Tags::RemoveTag( const std::string& tagName )
{
	std::vector<std::string>::iterator tagIterator;

	tagIterator = find(m_tags.begin(), m_tags.end(), tagName);
	if(tagIterator != m_tags.end())
	{
		m_tags.erase(tagIterator);
	}
	else
	{
		//The tag doesn't exist
		return;
	}

}

//------------------------------------------------------------------------------------------------------------------------------
bool Tags::HasTag( const std::string& tagName )
{
	std::vector<std::string>::iterator tagIterator;


	tagIterator = find(m_tags.begin(), m_tags.end(), tagName);
	if(tagIterator != m_tags.end())
	{
		if(tagName[0] == '!')
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if(tagName[0] == '!')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
const std::string Tags::GetAllTags() const
{
	std::string allTags = "";

	for(int tagIndex = 0; tagIndex < static_cast<int>(m_tags.size()); tagIndex++)
	{
		allTags += m_tags[tagIndex];
	}

	return allTags;
}