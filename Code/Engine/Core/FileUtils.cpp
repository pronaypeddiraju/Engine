#include "Engine/Core/FileUtils.hpp"
#include <fstream>

unsigned long CreateFileBuffer( std::string fileName, char **outData )
{
	std::ifstream fileStream;

	//Open the file in binary and at the end
	//This tells us where we are in the file and. We open in binary so it doesn't fuck with carriage returns
	//We open at end of file so it's easier to get the file size
	fileStream.open(fileName, std::ios::binary | std::ios::ate);

	if(fileStream.is_open())
	{
		//The file is open

		//tellg is an int telling us how far into the file we are. Since we are in the end we can assume this returns the file size
		unsigned long fileSize = (unsigned long)fileStream.tellg();
		fileStream.seekg(std::ios::beg);

		*outData = new char[fileSize];
		fileStream.read(*outData, fileSize);

		fileStream.close();
		return fileSize;
	}
	else
	{
		//We didn't open the file. Maybe the file doesn't exist
		//Don't assert the user because we could be opening a save file and we may not need to warn the user
		
		//Note: Remove this later when you don't need the assert anymore
		GUARANTEE_RECOVERABLE(true, "The file did not exist when creating File Buffer");
		return 0;
	}

}
