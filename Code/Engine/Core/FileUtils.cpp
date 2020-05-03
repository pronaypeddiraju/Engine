//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/FileUtils.hpp"
#include <fstream>

//------------------------------------------------------------------------------------------------------------------------------
unsigned long CreateFileReadBuffer(const std::string& fileName, char **outData )
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
		//Get the cursor back to the start of the file so we read the whole thing as opposed to reading nothing lol
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

//------------------------------------------------------------------------------------------------------------------------------
unsigned long CreateTextFileReadBuffer(const std::string& fileName, char **outData)
{
	std::ifstream fileStream;

	//We open at end of file so it's easier to get the file size
	fileStream.open(fileName, std::ios::ate);

	if (fileStream.is_open())
	{
		//The file is open

		//tellg is an int telling us how far into the file we are. Since we are in the end we can assume this returns the file size
		unsigned long fileSize = (unsigned long)fileStream.tellg();
		//Get the cursor back to the start of the file so we read the whole thing as opposed to reading nothing lol
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

//------------------------------------------------------------------------------------------------------------------------------
std::ofstream* CreateFileWriteBuffer(const std::string& fileName)
{
	std::ofstream* fileStream = new std::ofstream();

	//Open the file in binary and at the end
	//This tells us where we are in the file and. We open in binary so it doesn't fuck with carriage returns
	//We open at end of file so it's easier to get the file size
	fileStream->open(fileName, std::ios::binary | std::ios::ate);

	if (!fileStream->is_open())
	{
		//The file didn't exist so lets make one
		delete fileStream;
		fileStream = new std::ofstream(fileName, std::ofstream::out, std::ofstream::trunc);
	}

	return fileStream;
}

//------------------------------------------------------------------------------------------------------------------------------
std::ofstream* CreateTextFileWriteBuffer(const std::string& fileName)
{
	std::ofstream* fileStream = new std::ofstream();

	//We open at end of file so it's easier to get the file size
	fileStream->open(fileName, std::ios::ate);

	if (!fileStream->is_open())
	{
		//The file didn't exist so lets make one
		delete fileStream;
		fileStream = new std::ofstream(fileName, std::ofstream::out, std::ofstream::trunc);
	}

	return fileStream;
}

//------------------------------------------------------------------------------------------------------------------------------
std::string GetDirectoryFromFilePath(const std::string& filePath)
{
	//Iterate from the back and see if you can find a "/"
	for (uint i = (uint)filePath.size(); i > 0; --i)
	{
		if (filePath[i] == '/')
		{
			return std::string(filePath.begin(), filePath.begin() + i);
		}
	}
	if (filePath[0] == '/')
	{
		return "/";
	}
	else
	{
		return ".";
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool LoadBinaryFileToExistingBuffer(const std::string& filePath, std::vector<unsigned char>& outBuffer)
{
	FILE* file;
	long fileSize; 
	size_t result;

	//You wanna use fopen
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");	//Note: simply using r would be a read but rb is read binary format file

	//fread 
	if (errorCode != 0)
	{
		std::string errorMessage = Stringf("There was an error opening file %s", filePath.c_str());
		//ERROR_RECOVERABLE(errorMessage);
		return false;
	}
	else
	{
		//Check the size of the file. Ensure we reserve that much space on the vector
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		rewind(file);

		//Copy the buffer into the out_buffer
		outBuffer.resize(fileSize);
		result = fread(&outBuffer[0], sizeof(char), fileSize, file);	

		if (result != fileSize)
		{
			ERROR_RECOVERABLE("There was a problem reading the file");
			return false;
		}
	}

	//fclose when done
	fclose(file);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool SaveBinaryFileFromBuffer(const std::string& filePath, const std::vector<unsigned char>& writeBuffer)
{
	//fopen the file
	FILE* file;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");

	if (errorCode != 0)
	{
		std::string errorMessage = Stringf("There was an error opening file %s", filePath.c_str());
		ERROR_RECOVERABLE(errorMessage);
		return false;
	}
	else
	{
		//fwrite the buffer into the file
		fwrite(&writeBuffer[0], sizeof(char), writeBuffer.size(), file);
	}

	//fclose the file
	fclose(file);
	return true;
}
