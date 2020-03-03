//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
unsigned long				CreateFileReadBuffer(const std::string& fileName, char** outData);
unsigned long				CreateTextFileReadBuffer(const std::string& fileName, char** outData);

std::ofstream*				CreateFileWriteBuffer(const std::string& fileName);
std::ofstream*				CreateTextFileWriteBuffer(const std::string& fileName);

std::string					GetDirectoryFromFilePath(const std::string& filePath);

//We need to load binary files as buffers as well which will go under here
//NOTE: These functions will use fopen, fread, fwrite and fclose. Not streams
bool						LoadBinaryFileToExistingBuffer(const std::string& filePath, std::vector<unsigned char>& outBuffer);
bool						SaveBinaryFileFromBuffer(const std::string& filePath, const std::vector<unsigned char>& writeBuffer);