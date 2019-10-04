//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
unsigned long				CreateFileReadBuffer(const std::string& fileName, char** outData);
unsigned long				CreateTextFileReadBuffer(const std::string& fileName, char** outData);

std::ofstream*				CreateFileWriteBuffer(const std::string& fileName);
std::ofstream*				CreateTextFileWriteBuffer(const std::string& fileName);