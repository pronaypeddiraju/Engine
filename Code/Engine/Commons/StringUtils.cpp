//------------------------------------------------------------------------------------------------------------------------------
#include "StringUtils.hpp"
#include <stdarg.h>

//------------------------------------------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;

//------------------------------------------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}

//------------------------------------------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//------------------------------------------------------------------------------------------------------------------------------
std::vector<std::string> SplitStringOnDelimiter(std::string s, char delimiter )
{
	//Make a vector of strings
	std::vector<std::string> splitStrings;
	size_t startPos = 0;
	//find first occurance of the delimiter
	size_t endPos = s.find_first_of(delimiter);

	//Chop up the dude into separate strings
	while( endPos != s.npos)
	{
		std::string tempString = s.substr(startPos, endPos-startPos);
		splitStrings.push_back(tempString);
		startPos = endPos + 1;
		endPos = s.find_first_of(delimiter, startPos);
	}

	//Shove the last sub string into the guy
	splitStrings.push_back(s.substr(startPos, s.size()));
	//Send him home
	return splitStrings;

}